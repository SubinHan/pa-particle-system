#include "Core/ParticlePass.h"

#include "Core/DxDevice.h"
#include "Core/HlslGeneratorRender.h"
#include "Core/ParticleResource.h"
#include "Core/PassConstantBuffer.h"
#include "Model/Material.h"
#include "Model/ObjectConstants.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/";
static const std::wstring BASE_RENDER_SHADER_PATH = L"ParticleSystemShaders/ParticleRenderBase.hlsl";

using Microsoft::WRL::ComPtr;

constexpr int ROOT_SLOT_OBJECT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = ROOT_SLOT_OBJECT_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_DIFFUSE_MAP_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER + 1;

constexpr int ROOT_SLOT_SRV_UAV_TABLE = 0;

ParticlePass::ParticlePass(DxDevice* device, ParticleResource* resource) :
	_device(device),
	_resource(resource),
	_hlslGenerator(std::make_unique<HlslGeneratorRender>(BASE_RENDER_SHADER_PATH))
{
	buildRootSignature();
	buildCommandSignature();
	buildShaders();
	buildInputLayout();
	buildPsos();
	generateEmptyGeometry();
}

void ParticlePass::setMaterial(Material* material)
{
	_material = material;
}

void ParticlePass::render(
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

	cmdList->SetPipelineState(_psoTransparency.Get());
	cmdList->SetGraphicsRoot32BitConstants(ROOT_SLOT_OBJECT_CONSTANTS_BUFFER, sizeof(ObjectConstants) / 4, &objectConstants, 0);
	cmdList->SetGraphicsRootDescriptorTable(
		ROOT_SLOT_PASS_CONSTANTS_BUFFER, passCb.getGpuHandle());
	cmdList->SetGraphicsRootShaderResourceView(
		ROOT_SLOT_PARTICLES_BUFFER, _resource->getParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootShaderResourceView(
		ROOT_SLOT_ALIVES_INDICES_BUFFER, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootDescriptorTable(
		ROOT_SLOT_DIFFUSE_MAP_BUFFER, _material->DiffuseSrvHandle);

	_resource->transitCommandBufferToIndirectArgument(cmdList);

	// TODO: change to executeindirect
	cmdList->ExecuteIndirect(
		_commandSignature.Get(),
		_resource->getMaxNumParticles(),
		_resource->getIndirectCommandsResource(),
		0,
		_resource->getIndirectCommandsResource(),
		_resource->getCommandBufferCounterOffset());

	_resource->transitParticlesToUav(cmdList);
	_resource->transitAliveIndicesToUav(cmdList);
}

void ParticlePass::compileShaders()
{
	const std::wstring shaderPath = SHADER_ROOT_PATH + std::to_wstring(_hash) + L".hlsl";

	_hlslGenerator->compile(shaderPath);

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
}

void ParticlePass::buildRootSignature()
{
	// build graphics root signature
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[5];
		slotRootParameter[ROOT_SLOT_OBJECT_CONSTANTS_BUFFER].InitAsConstants(sizeof(ObjectConstants) / 4, 0);

		CD3DX12_DESCRIPTOR_RANGE passCbvTable;
		passCbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
		slotRootParameter[ROOT_SLOT_PASS_CONSTANTS_BUFFER].InitAsDescriptorTable(1, &passCbvTable);

		slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER].InitAsShaderResourceView(0); // particles
		slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER].InitAsShaderResourceView(1); // aliveIndices

		CD3DX12_DESCRIPTOR_RANGE texSrvTable;
		texSrvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
		slotRootParameter[ROOT_SLOT_DIFFUSE_MAP_BUFFER].InitAsDescriptorTable(1, &texSrvTable);

		auto staticSamplers = DxUtil::getStaticSamplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
			_countof(slotRootParameter),
			slotRootParameter,
			staticSamplers.size(),
			staticSamplers.data(),
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

		ThrowIfFailed(_device->getD3dDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&_rootSignature))
		);
	}

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

		ThrowIfFailed(_device->getD3dDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&_computeRootSignature)));
	}
}

void ParticlePass::buildCommandSignature()
{
	D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[1] = {};
	argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
	commandSignatureDesc.pArgumentDescs = argumentDescs;
	commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
	commandSignatureDesc.ByteStride = sizeof(ParticleIndirectCommand);

	ThrowIfFailed(_device->getD3dDevice()->CreateCommandSignature(
		&commandSignatureDesc, nullptr, IID_PPV_ARGS(&_commandSignature)));
}

void ParticlePass::buildShaders()
{
	_shaderIndirectCommand = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticleComputeIndirectCommands.hlsl",
		nullptr,
		"ComputeIndirectCommandsCS",
		"cs_5_1");

	UINT textureSampleIndex = _hlslGenerator->sampleTexture2d();
	UINT alphaThresholdIndex = _hlslGenerator->newFloat(0.5f);
	UINT textureAlphaIndex = _hlslGenerator->maskX(textureSampleIndex);
	_hlslGenerator->clip(textureAlphaIndex);
	UINT outputColorIndex = _hlslGenerator->newFloat4(1.0f, 0.00f, 0.00f, 1.0f);
	UINT alphaModifiedOutputColorIndex = _hlslGenerator->setAlpha(outputColorIndex, textureAlphaIndex);
	_hlslGenerator->setOutputColor(alphaModifiedOutputColorIndex);

	compileShaders();
}

void ParticlePass::buildInputLayout()
{
	_inputLayout =
	{
	};
}

void ParticlePass::buildPsos()
{
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
	opaquePsoDesc.RTVFormats[0] = _device->getBackBufferFormat();
	bool msaaState = _device->getMsaaState();
	opaquePsoDesc.SampleDesc.Count = msaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = msaaState ? _device->getMsaaQuality() - 1 : 0;
	opaquePsoDesc.DSVFormat = _device->getDepthStencilFormat();
	ThrowIfFailed(_device->getD3dDevice()->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&_psoOpaque)));


	auto transparentPsoDesc = opaquePsoDesc;

	transparentPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;
	transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparencyBlendDesc.DestBlend = D3D12_BLEND_DEST_ALPHA;
	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	ThrowIfFailed(
		_device->getD3dDevice()->CreateGraphicsPipelineState(
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
		_device->getD3dDevice()->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_psoCompute)));
}

void ParticlePass::generateEmptyGeometry()
{
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
		_device->getD3dDevice().Get(),
		_device->getCommandList().Get(),
		vertices.data(),
		vbByteSize,
		_emptyGeometry->VertexBufferUploader
	);

	_emptyGeometry->IndexBufferGPU = DxUtil::createDefaultBuffer(
		_device->getD3dDevice().Get(),
		_device->getCommandList().Get(),
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
