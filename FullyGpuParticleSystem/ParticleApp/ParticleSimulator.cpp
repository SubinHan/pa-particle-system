#include "ParticleSimulator.h"

#include "ParticleResource.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

ParticleSimulator::ParticleSimulator(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource) :
	_device(device),
	_resource(resource)
{
	buildRootSignature();
	buildShaders();
	buildPsos();
}

ID3D12RootSignature* ParticleSimulator::getRootSignature()
{
	return _rootSignature.Get();
}

ID3DBlob* ParticleSimulator::getShader()
{
	return _shader.Get();
}

ID3D12PipelineState* ParticleSimulator::getPipelineStateObject()
{
	return _pso.Get();
}

void ParticleSimulator::simulateParticles(ID3D12GraphicsCommandList* cmdList, float deltaTime)
{
	ParticleSimulateConstants c = { deltaTime };

	cmdList->SetComputeRootSignature(_rootSignature.Get());

	cmdList->SetPipelineState(_pso.Get());

	cmdList->SetComputeRoot32BitConstants(ROOT_SLOT_CONSTANTS_BUFFER, 1, &c, 0);
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_PARTICLES_BUFFER, _resource->getParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK, _resource->getAliveIndicesResourceBack()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_DEADS_INDICES_BUFFER, _resource->getDeadIndicesResource()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_COUNTERS_BUFFER, _resource->getCountersResource()->GetGPUVirtualAddress());

	const auto numGroupsX = static_cast<UINT>(ceilf(_resource->getMaxNumParticles() / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
	
	_resource->swapAliveIndicesBuffer();
}

void ParticleSimulator::buildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[6];

	slotRootParameter[ROOT_SLOT_CONSTANTS_BUFFER]
		.InitAsConstants(1, 0);
	slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER]
		.InitAsUnorderedAccessView(0);
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT]
		.InitAsUnorderedAccessView(1);
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK]
		.InitAsUnorderedAccessView(2);
	slotRootParameter[ROOT_SLOT_DEADS_INDICES_BUFFER]
		.InitAsUnorderedAccessView(3);
	slotRootParameter[ROOT_SLOT_COUNTERS_BUFFER]
		.InitAsUnorderedAccessView(4);

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

	ThrowIfFailed(_device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.GetAddressOf())));
}

void ParticleSimulator::buildShaders()
{
	_shader = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticleSimulateCS.hlsl",
		nullptr,
		"SimulateCS",
		"cs_5_1");
}

void ParticleSimulator::buildPsos()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = _rootSignature.Get();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shader->GetBufferPointer()),
		_shader->GetBufferSize()
	};
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(
		_device->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_pso)));
}
