#include "Core/ParticlePass.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/PassConstantBuffer.h"
#include "Model/Material.h"
#include "Model/ObjectConstants.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

constexpr int ROOT_SLOT_OBJECT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = ROOT_SLOT_OBJECT_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_DIFFUSE_MAP_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER + 1;

ParticlePass::ParticlePass(DxDevice* device, ParticleResource* resource) :
	_device(device),
	_resource(resource)
{
	buildRootSignature();
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
	cmdList->SetGraphicsRootSignature(_rootSignature.Get());

	auto vertexBuffers = _emptyGeometry->VertexBufferView();
	auto indexBuffer = _emptyGeometry->IndexBufferView();

	cmdList->IASetVertexBuffers(0, 1, &vertexBuffers);
	cmdList->IASetIndexBuffer(&indexBuffer);

	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	_resource->transitParticlesToSrv(cmdList);
	_resource->transitAliveIndicesToSrv(cmdList);

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

	// TODO: change to executeindirect
	cmdList->DrawIndexedInstanced(
		_resource->getMaxNumParticles(),
		1,
		0,
		0,
		0);

	_resource->transitParticlesToUav(cmdList);
	_resource->transitAliveIndicesToUav(cmdList);
}

void ParticlePass::buildRootSignature()
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

void ParticlePass::buildShaders()
{
	_shaderVs = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticleRender.hlsl",
		nullptr,
		"ParticleVS",
		"vs_5_1");
	_shaderGs = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticleRender.hlsl",
		nullptr,
		"ParticleGS",
		"gs_5_1");
	_shaderPs = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticleRender.hlsl",
		nullptr,
		"ParticlePS",
		"ps_5_1");
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
