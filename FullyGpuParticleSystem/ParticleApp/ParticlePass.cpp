#include "ParticlePass.h"

#include "Core/DxDevice.h"
#include "ParticleResource.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

ParticlePass::ParticlePass(DxDevice* device, ParticleResource* resource) :
	_device(device),
	_resource(resource)
{
	buildRootSignature();
	buildShaders();
	buildInputLayout();
	buildPsos();
}

void ParticlePass::render(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootSignature(_rootSignature.Get());

	cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	cmdList->SetPipelineState(_pso.Get());
	//cmdList->SetGraphicsRootDescriptorTable(0, );
	cmdList->SetGraphicsRootShaderResourceView(
		1, _resource->getParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootShaderResourceView(
		2, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());

	// TODO: change to executeindirect
	cmdList->DrawIndexedInstanced(
		100,
		1,
		0,
		0,
		0);

}

void ParticlePass::buildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	slotRootParameter[1].InitAsShaderResourceView(0); // particles
	slotRootParameter[2].InitAsShaderResourceView(1); // aliveIndices

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		1,
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
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { _inputLayout.data(), static_cast<UINT>(_inputLayout.size()) };
	psoDesc.pRootSignature = _rootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(_shaderVs->GetBufferPointer()),
		_shaderVs->GetBufferSize()
	};
	psoDesc.GS =
	{
		reinterpret_cast<BYTE*>(_shaderGs->GetBufferPointer()),
		_shaderGs->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(_shaderPs->GetBufferPointer()),
		_shaderPs->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = _device->getBackBufferFormat();
	bool msaaState = _device->getMsaaState();
	psoDesc.SampleDesc.Count = msaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = msaaState ? _device->getMsaaQuality() - 1 : 0;
	psoDesc.DSVFormat = _device->getDepthStencilFormat();
	ThrowIfFailed(_device->getD3dDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pso)));
}
