#include "ParticleEmitter.h"

#include "ParticleResource.h"
#include "Util/DxDebug.h"
#include "Util/DxUtil.h"

#include "d3dx12.h"
#include "wrl.h"

using Microsoft::WRL::ComPtr;

ParticleEmitter::ParticleEmitter(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource) :
		_device(device),
		_resource(resource)
{
	buildRootSignature();
	buildShaders();
	buildPsos();
}

ID3D12RootSignature* ParticleEmitter::getRootSignature()
{
	return _rootSignature.Get();
}

ID3DBlob* ParticleEmitter::getShader()
{
	return _shader.Get();
}

ID3D12PipelineState* ParticleEmitter::getPipelineStateObject()
{
	return _pso.Get();
}

void ParticleEmitter::emitParticles(ID3D12GraphicsCommandList* cmdList, int numParticlesToEmit)
{
	EmitConstants c = { numParticlesToEmit, DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f}, DirectX::XMFLOAT2{0.0f, 0.0f} };

	cmdList->SetComputeRootSignature(_rootSignature.Get());

	//const auto particlesToUav = CD3DX12_RESOURCE_BARRIER::Transition(
	//	_resource->getParticlesResource(),
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//cmdList->ResourceBarrier(1, &particlesToUav);

	cmdList->SetPipelineState(_pso.Get());

	cmdList->SetComputeRoot32BitConstants(ROOT_SLOT_CONSTANTS_BUFFER, 6, &c, 0);
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_PARTICLES_BUFFER, _resource->getParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_ALIVES_INDICES_BUFFER, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_DEADS_INDICES_BUFFER, _resource->getDeadIndicesResource()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_COUNTERS_BUFFER, _resource->getCountersResource()->GetGPUVirtualAddress());

	const auto numGroupsX = static_cast<UINT>(ceilf(numParticlesToEmit / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);

	//const auto particlesToSrv = CD3DX12_RESOURCE_BARRIER::Transition(
	//	_resource->getParticlesResource(),
	//	D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
	//	D3D12_RESOURCE_STATE_GENERIC_READ);
	//cmdList->ResourceBarrier(1, &particlesToSrv);
}

void ParticleEmitter::buildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	slotRootParameter[ROOT_SLOT_CONSTANTS_BUFFER]
		.InitAsConstants(6, 0);
	slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER]
		.InitAsUnorderedAccessView(0);
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER]
		.InitAsUnorderedAccessView(1);
	slotRootParameter[ROOT_SLOT_DEADS_INDICES_BUFFER]
		.InitAsUnorderedAccessView(2);
	slotRootParameter[ROOT_SLOT_COUNTERS_BUFFER]
		.InitAsUnorderedAccessView(3);

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

void ParticleEmitter::buildShaders()
{
	_shader = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticleEmitCS.hlsl",
		nullptr,
		"EmitCS",
		"cs_5_1");
}

void ParticleEmitter::buildPsos()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC pso = {};
	pso.pRootSignature = _rootSignature.Get();
	pso.CS =
	{
		reinterpret_cast<BYTE*>(_shader->GetBufferPointer()),
		_shader->GetBufferSize()
	};
	pso.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(
		_device->CreateComputePipelineState(
			&pso, IID_PPV_ARGS(&_pso)));
}
