#include "Core/ParticleRenderer.h"

#include "Core/DxDevice.h"
#include "Core/HlslGeneratorRender.h"
#include "Core/ParticleResource.h"
#include "Core/PassConstantBuffer.h"
#include "Core/MaterialManager.h"
#include "Core/TextureManager.h"
#include "Model/Material.h"
#include "Model/ObjectConstants.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";
static const std::wstring BASE_RENDER_SHADER_PATH = L"ParticleSystemShaders/ParticleRenderBase.hlsl";

using Microsoft::WRL::ComPtr;

constexpr int ROOT_SLOT_OBJECT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = ROOT_SLOT_OBJECT_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_DIFFUSE_MAP_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER + 1;

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

void ParticleRenderer::render(
	ID3D12GraphicsCommandList* cmdList,
	const ObjectConstants& objectConstants,
	const PassConstantBuffer& passCb)
{
	cmdList->SetComputeRootSignature(_computeRootSignature.Get());
	cmdList->SetPipelineState(_psoCompute.Get());

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

	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	cmdList->SetPipelineState(_isOpaque ? _psoOpaque.Get() : _psoTransparency.Get());
	cmdList->SetGraphicsRoot32BitConstants(ROOT_SLOT_OBJECT_CONSTANTS_BUFFER, sizeof(ObjectConstants) / 4, &objectConstants, 0);
	cmdList->SetGraphicsRootDescriptorTable(
		ROOT_SLOT_PASS_CONSTANTS_BUFFER, passCb.getGpuHandle());
	cmdList->SetGraphicsRootShaderResourceView(
		ROOT_SLOT_PARTICLES_BUFFER, _resource->getParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootShaderResourceView(
		ROOT_SLOT_ALIVES_INDICES_BUFFER, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());

	bindGraphicsResourcesOfRegisteredNodes(cmdList, 4);

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
		"ParticleVS",
		"vs_5_1");

	_shaderGs = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"ParticleGS",
		"gs_5_1");

	_shaderPs = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"ParticlePS",
		"ps_5_1");

	_shaderIndirectCommand = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticleComputeIndirectCommands.hlsl",
		nullptr,
		"ComputeIndirectCommandsCS",
		"cs_5_1");
}

void ParticleRenderer::setShaderPs(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shaderPs = shader;
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
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(4);
	slotRootParameter[ROOT_SLOT_OBJECT_CONSTANTS_BUFFER].InitAsConstants(sizeof(ObjectConstants) / 4, 0);

	_passCbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	slotRootParameter[ROOT_SLOT_PASS_CONSTANTS_BUFFER].InitAsDescriptorTable(1, &_passCbvTable);

	slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER].InitAsShaderResourceView(0); // particles
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER].InitAsShaderResourceView(1); // aliveIndices

	// build compute root signature
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

		auto device = DxDevice::getInstance().getD3dDevice();

		ThrowIfFailed(device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&_computeRootSignature)));
	}
	
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
	setShaderPs(_shaderPs);
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

	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = _computeRootSignature.Get();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shaderIndirectCommand->GetBufferPointer()),
		_shaderIndirectCommand->GetBufferSize()
	};
	ThrowIfFailed(
		device.getD3dDevice()->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_psoCompute)));
}

void ParticleRenderer::generateEmptyGeometry()
{
	auto device = DxDevice::getInstance();

	using VertexType = DirectX::XMFLOAT3;
	using IndexType = std::uint32_t;

	auto maxNumParticles = _resource->getMaxNumParticles();
	std::vector<VertexType> vertices(1);
	std::vector<IndexType> indices(maxNumParticles);

	for (int i = 0; i < indices.size(); ++i)
	{
		indices[i] = i;
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
