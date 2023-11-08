#include "Core/ParticleRenderer.h"

#include "Core/DxDevice.h"
#include "Core/HlslGeneratorRender.h"
#include "Core/ParticleResource.h"
#include "Core/PassConstantBuffer.h"
#include "Core/MaterialManager.h"
#include "Core/TextureManager.h"
#include "Model/Material.h"
#include "Model/ObjectConstants.h"
#include "Model/RendererType.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";
static const std::wstring BASE_RENDER_SHADER_PATH = L"ParticleSystemShaders/ParticleRenderBase.hlsl";

using Microsoft::WRL::ComPtr;

constexpr int ROOT_SLOT_OBJECT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = ROOT_SLOT_OBJECT_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER + 1;

constexpr int ROOT_SLOT_SRV_UAV_TABLE = 0;

ParticleRenderer::ParticleRenderer(ParticleResource* resource, std::string name) :
	ParticlePass(resource, name),
	_materialName("default"),
	_hlslGenerator(std::make_unique<HlslGeneratorRender>(BASE_RENDER_SHADER_PATH))
{
	buildCommandSignature();
	buildInputLayout();
	generateEmptyGeometry();
	buildDefaultShader();
}

void ParticleRenderer::setMaterialName(std::string materialName)
{
	_materialName = materialName;
}

void ParticleRenderer::setRendererType(RendererType type)
{
	_currentRendererType = type;
}

void ParticleRenderer::render(
	ID3D12GraphicsCommandList* cmdList,
	const ObjectConstants& objectConstants,
	const PassConstantBuffer& passCb)
{
	if (_currentRendererType == RendererType::Ribbon)
	{
		// calculate ribbon distance from start

		cmdList->SetComputeRootSignature(_ribbonDistanceRootSignature.Get());
		cmdList->SetPipelineState(_psoPreRibbonDistance.Get());
		cmdList->SetComputeRootUnorderedAccessView(
			1,
			_resource->getParticlesResource()->GetGPUVirtualAddress());
		cmdList->SetComputeRootUnorderedAccessView(
			2,
			_resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());

		cmdList->Dispatch(static_cast<UINT>(ceil(static_cast<float>(_resource->getMaxNumParticles()) / 256.0f)), 1, 1);

		UINT numParticles = _resource->getMaxNumParticles() + 1;

		cmdList->SetPipelineState(_psoRibbonDistance.Get());
		cmdList->SetComputeRoot32BitConstants(
			0,
			1,
			&numParticles,
			0
		);
		cmdList->SetComputeRootUnorderedAccessView(
			1,
			_resource->getParticlesResource()->GetGPUVirtualAddress());
		cmdList->SetComputeRootUnorderedAccessView(
			2,
			_resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());
		cmdList->Dispatch(static_cast<UINT>(ceil(static_cast<float>(_resource->getMaxNumParticles()) / 256.0f)), 1, 1);
	}

	cmdList->SetComputeRootSignature(_computeRootSignature.Get());
	cmdList->SetPipelineState(getCurrentComputePso());

	cmdList->SetComputeRootDescriptorTable(
		0,
		_resource->getCountersUavGpuHandle());
	cmdList->SetComputeRootDescriptorTable(
		1,
		_resource->getIndirectCommandsUavGpuHandle());

	D3D12_RESOURCE_BARRIER toCopy =
		CD3DX12_RESOURCE_BARRIER::Transition(
			_resource->getIndirectCommandsResource(),
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
			D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->ResourceBarrier(1, &toCopy);

	cmdList->CopyBufferRegion(
		_resource->getIndirectCommandsResource(),
		_resource->getCommandBufferCounterOffset(),
		_resource->getIndirectCommandsCounterResetResource(),
		0,
		sizeof(UINT));

	D3D12_RESOURCE_BARRIER toUav =
		CD3DX12_RESOURCE_BARRIER::Transition(
			_resource->getIndirectCommandsResource(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cmdList->ResourceBarrier(1, &toUav);

	cmdList->Dispatch(static_cast<UINT>(ceil(static_cast<float>(_resource->getMaxNumParticles()) / 256.0f)), 1, 1);

	//

	_resource->transitParticlesToSrv(cmdList);
	_resource->transitAliveIndicesToSrv(cmdList);

	cmdList->SetGraphicsRootSignature(_rootSignature.Get());

	auto vertexBuffers = _emptyGeometry->VertexBufferView();
	auto indexBuffer = _emptyGeometry->IndexBufferView();

	cmdList->IASetVertexBuffers(0, 1, &vertexBuffers);
	cmdList->IASetIndexBuffer(&indexBuffer);

	cmdList->IASetPrimitiveTopology(getCurrentPrimitiveTopology());

	cmdList->SetPipelineState(getCurrentPso());
	cmdList->SetGraphicsRoot32BitConstants(ROOT_SLOT_OBJECT_CONSTANTS_BUFFER, sizeof(ObjectConstants) / 4, &objectConstants, 0);
	cmdList->SetGraphicsRootDescriptorTable(
		ROOT_SLOT_PASS_CONSTANTS_BUFFER, passCb.getGpuHandle());
	cmdList->SetGraphicsRootShaderResourceView(
		ROOT_SLOT_PARTICLES_BUFFER, _resource->getParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootShaderResourceView(
		ROOT_SLOT_ALIVES_INDICES_BUFFER, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootDescriptorTable(
		ROOT_SLOT_COUNTERS_BUFFER, _resource->getCountersUavGpuHandle());

	bindGraphicsResourcesOfRegisteredNodes(cmdList, 5);

	_resource->transitCommandBufferToIndirectArgument(cmdList);

	// TODO: Optimization - Set proper NumMaxCommands.
	cmdList->ExecuteIndirect(
		_commandSignature.Get(),
		_resource->getMaxNumParticles(),
		//1000,
		_resource->getIndirectCommandsResource(),
		0,
		_resource->getIndirectCommandsResource(),
		_resource->getCommandBufferCounterOffset());

	_resource->transitParticlesToUav(cmdList);
	_resource->transitAliveIndicesToUav(cmdList);
}

void ParticleRenderer::compileShaders()
{
	const std::wstring shaderPath = SHADER_ROOT_PATH + std::to_wstring(_hash) + L".hlsl";

	_hlslGenerator->generateShaderFile(shaderPath);

	_shaderVs = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"SpriteParticleVS",
		"vs_5_1");

	_shaderGs = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"SpriteParticleGS",
		"gs_5_1");

	_shaderVsRibbon = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"RibbonParticleVS",
		"vs_5_1");

	_shaderHsRibbon = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"RibbonParticleHS",
		"hs_5_1");

	_shaderDsRibbon = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"RibbonParticleDS",
		"ds_5_1");

	_shaderPs = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"ParticlePS",
		"ps_5_1");

	_shaderIndirectCommand = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleComputeIndirectCommands.hlsl",
		nullptr,
		"ComputeIndirectCommandsCS",
		"cs_5_1");

	_shaderIndirectCommandRibbon = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleComputeIndirectCommands.hlsl",
		nullptr,
		"RibbonComputeIndirectCommandsCS",
		"cs_5_1");

	_shaderPreRibbonDistance = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticlePreRibbonDistanceCS.hlsl",
		nullptr,
		"PreRibbonDistanceCS",
		"cs_5_1");

	_shaderRibbonDistance = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleRibbonDistanceCS.hlsl",
		nullptr,
		"BrentKung",
		"cs_5_1");
}

void ParticleRenderer::setSpritePixelShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderPs = shader;
	buildRootSignature();
	buildPsos();
}

void ParticleRenderer::setRibbonPixelShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderPsRibbon = shader;
	buildRootSignature();
	buildPsos();
}

bool ParticleRenderer::isOpaque()
{
	return _isOpaque;
}

void ParticleRenderer::setOpaque(bool newIsOpaque)
{
	_isOpaque = newIsOpaque;
}

std::vector<CD3DX12_ROOT_PARAMETER> ParticleRenderer::buildRootParameter()
{
	auto device = DxDevice::getInstance().getD3dDevice();

	// build compute root signature for sprite
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[2];

		CD3DX12_DESCRIPTOR_RANGE counterTable;
		counterTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

		slotRootParameter[0]
			.InitAsDescriptorTable(1, &counterTable);

		CD3DX12_DESCRIPTOR_RANGE uavTable;
		uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);

		slotRootParameter[1]
			.InitAsDescriptorTable(1, &uavTable);

		auto staticSamplers = DxUtil::getStaticSamplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
			_countof(slotRootParameter),
			slotRootParameter,
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(
			&rootSigDesc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(),
			errorBlob.GetAddressOf()
		);

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&_computeRootSignature)));
	}

	// build compute root signature for ribbon distance
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[3];

		slotRootParameter[0]
			.InitAsConstants(1, 0);
		slotRootParameter[1]
			.InitAsUnorderedAccessView(0);
		slotRootParameter[2]
			.InitAsUnorderedAccessView(1);

		//CD3DX12_DESCRIPTOR_RANGE uavTable;
		//uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
		//slotRootParameter[SPAWN_ROOT_SLOT_COUNTERS_BUFFER]
		//	.InitAsDescriptorTable(1, &uavTable);

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
			_countof(slotRootParameter),
			slotRootParameter,
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_NONE);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(_ribbonDistanceRootSignature.GetAddressOf())));
	}

	// root parameter for renderbase
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(5);
	slotRootParameter[ROOT_SLOT_OBJECT_CONSTANTS_BUFFER].InitAsConstants(sizeof(ObjectConstants) / 4, 0);

	_passCbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	slotRootParameter[ROOT_SLOT_PASS_CONSTANTS_BUFFER].InitAsDescriptorTable(1, &_passCbvTable);

	slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER].InitAsShaderResourceView(0); // particles
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER].InitAsShaderResourceView(1); // aliveIndices

	_counterTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
	slotRootParameter[ROOT_SLOT_COUNTERS_BUFFER].InitAsDescriptorTable(1, &_counterTable);

	return slotRootParameter;
}

int ParticleRenderer::getNumSrvUsing()
{
	return 2;
}

int ParticleRenderer::getNumUavUsing()
{
	return 0;
}

bool ParticleRenderer::needsStaticSampler()
{
	return true;
}

void ParticleRenderer::buildCommandSignature()
{
	D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[1] = {};
	argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
	commandSignatureDesc.pArgumentDescs = argumentDescs;
	commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
	commandSignatureDesc.ByteStride = sizeof(ParticleIndirectCommand);

	auto device = DxDevice::getInstance().getD3dDevice();

	ThrowIfFailed(device->CreateCommandSignature(
		&commandSignatureDesc, nullptr, IID_PPV_ARGS(&_commandSignature)));
}

void ParticleRenderer::buildDefaultShader()
{
	UINT outputColorIndex = _hlslGenerator->newFloat4(1.0f, 1.00f, 1.00f, 0.04f);
	_hlslGenerator->setOutputColor(outputColorIndex);

	compileShaders();
	setSpritePixelShader(_shaderPs);
}

void ParticleRenderer::buildInputLayout()
{
	_inputLayout =
	{
	};
}

void ParticleRenderer::buildPsos()
{
	auto device = DxDevice::getInstance();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { _inputLayout.data(), static_cast<UINT>(_inputLayout.size()) };
	opaquePsoDesc.pRootSignature = _rootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(_shaderVs->GetBufferPointer()),
		_shaderVs->GetBufferSize()
	};
	opaquePsoDesc.GS =
	{
		reinterpret_cast<BYTE*>(_shaderGs->GetBufferPointer()),
		_shaderGs->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(_shaderPs->GetBufferPointer()),
		_shaderPs->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = device.getBackBufferFormat();
	bool msaaState = device.getMsaaState();
	opaquePsoDesc.SampleDesc.Count = msaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = msaaState ? device.getMsaaQuality() - 1 : 0;
	opaquePsoDesc.DSVFormat = device.getDepthStencilFormat();
	ThrowIfFailed(device.getD3dDevice()->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&_psoOpaque)));

	auto transparentPsoDesc = opaquePsoDesc;

	transparentPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;
	transparencyBlendDesc.SrcBlend = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlend = D3D12_BLEND_ONE;
	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_INV_DEST_ALPHA;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	ThrowIfFailed(
		device.getD3dDevice()->CreateGraphicsPipelineState(
			&transparentPsoDesc, IID_PPV_ARGS(&_psoTransparency)
		)
	);

	//////////////// Ribbon
	auto ribbonPsoDesc = transparentPsoDesc;
	ribbonPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(_shaderVsRibbon->GetBufferPointer()),
		_shaderVsRibbon->GetBufferSize()
	};
	ribbonPsoDesc.HS =
	{
		reinterpret_cast<BYTE*>(_shaderHsRibbon->GetBufferPointer()),
		_shaderHsRibbon->GetBufferSize()
	};
	ribbonPsoDesc.DS =
	{
		reinterpret_cast<BYTE*>(_shaderDsRibbon->GetBufferPointer()),
		_shaderDsRibbon->GetBufferSize()
	};
	ribbonPsoDesc.GS =
	{
		nullptr,
		0
	};

	if (_shaderPsRibbon)
	{
		ribbonPsoDesc.PS =
		{
			reinterpret_cast<BYTE*>(_shaderPsRibbon->GetBufferPointer()),
			_shaderPsRibbon->GetBufferSize()
		};
	}
	else
	{
		ribbonPsoDesc.PS =
		{
			reinterpret_cast<BYTE*>(_shaderPs->GetBufferPointer()),
			_shaderPs->GetBufferSize()
		};
	}
	ribbonPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	ThrowIfFailed(
		device.getD3dDevice()->CreateGraphicsPipelineState(
			&ribbonPsoDesc, IID_PPV_ARGS(&_psoRibbon)
		)
	);

	//////////////// ComputePso
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = _computeRootSignature.Get();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shaderIndirectCommand->GetBufferPointer()),
		_shaderIndirectCommand->GetBufferSize()
	};
	ThrowIfFailed(
		device.getD3dDevice()->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_psoComputeIndirect)));

	//////////////// ComputePso for Ribbon
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shaderIndirectCommandRibbon->GetBufferPointer()),
		_shaderIndirectCommandRibbon->GetBufferSize()
	};
	ThrowIfFailed(
		device.getD3dDevice()->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_psoComputeIndirectRibbon)));

	psoDesc.pRootSignature = _ribbonDistanceRootSignature.Get();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shaderPreRibbonDistance->GetBufferPointer()),
		_shaderPreRibbonDistance->GetBufferSize()
	};
	ThrowIfFailed(
		device.getD3dDevice()->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_psoPreRibbonDistance)));

	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shaderRibbonDistance->GetBufferPointer()),
		_shaderRibbonDistance->GetBufferSize()
	};
	ThrowIfFailed(
		device.getD3dDevice()->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_psoRibbonDistance)));
}

void ParticleRenderer::generateEmptyGeometry()
{
	auto device = DxDevice::getInstance();

	using VertexType = DirectX::XMFLOAT3;
	using IndexType = std::uint32_t;

	auto maxNumParticles = _resource->getMaxNumParticles() + 1;
	std::vector<VertexType> vertices(1);
	std::vector<IndexType> indices(maxNumParticles);

	indices[0] = 0;
	for (int i = 1; i < indices.size(); ++i)
	{
		indices[i] = i - 1;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexType);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(IndexType);

	_emptyGeometry = std::make_unique<MeshGeometry>();
	_emptyGeometry->Name = "emptyGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &_emptyGeometry->VertexBufferCPU));
	CopyMemory(_emptyGeometry->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &_emptyGeometry->IndexBufferCPU));
	CopyMemory(_emptyGeometry->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	_emptyGeometry->VertexBufferGPU = DxUtil::createDefaultBuffer(
		device.getD3dDevice().Get(),
		device.getCommandList().Get(),
		vertices.data(),
		vbByteSize,
		_emptyGeometry->VertexBufferUploader
	);

	_emptyGeometry->IndexBufferGPU = DxUtil::createDefaultBuffer(
		device.getD3dDevice().Get(),
		device.getCommandList().Get(),
		indices.data(),
		ibByteSize,
		_emptyGeometry->IndexBufferUploader
	);

	_emptyGeometry->VertexByteStride = sizeof(VertexType);
	_emptyGeometry->VertexBufferByteSize = vbByteSize;
	_emptyGeometry->IndexFormat = DXGI_FORMAT_R32_UINT;
	_emptyGeometry->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	_emptyGeometry->DrawArgs["empty"] = submesh;
}

ID3D12PipelineState* ParticleRenderer::getCurrentPso()
{
	if (_currentRendererType == RendererType::Ribbon)
		return _psoRibbon.Get();

	if (_currentRendererType == RendererType::Sprite)
		return _isOpaque ? _psoOpaque.Get() : _psoTransparency.Get();
}

ID3D12PipelineState* ParticleRenderer::getCurrentComputePso()
{
	return _currentRendererType == RendererType::Sprite ? _psoComputeIndirect.Get() : _psoComputeIndirectRibbon.Get();
}

D3D12_PRIMITIVE_TOPOLOGY ParticleRenderer::getCurrentPrimitiveTopology()
{
	return _currentRendererType == RendererType::Sprite ? D3D11_PRIMITIVE_TOPOLOGY_POINTLIST : D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
}
