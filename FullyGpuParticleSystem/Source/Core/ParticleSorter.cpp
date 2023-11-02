#include "Core/ParticleSorter.h"

#include "Core/ParticleResource.h"

using namespace Microsoft::WRL;

constexpr int ROOT_SLOT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_ALIVE_INDICES_BUFFER = ROOT_SLOT_CONSTANTS_BUFFER + 1;

ParticleSorter::ParticleSorter(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource, std::string name) :
	_device(device),
	_resource(resource),
	_name(name)
{
	buildRootSignature();
	buildShaders();
	buildPsos();
}

ID3D12RootSignature* ParticleSorter::getRootSignature()
{
	return _rootSignature.Get();
}

ID3DBlob* ParticleSorter::getShader()
{
	return _shader.Get();
}

ID3D12PipelineState* ParticleSorter::getPipelineStateObject()
{
	return _pso.Get();
}

void ParticleSorter::sortParticles(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetComputeRootSignature(_rootSignature.Get());

	cmdList->SetPipelineState(_pso.Get());

	cmdList->SetComputeRootUnorderedAccessView(
		ROOT_SLOT_ALIVE_INDICES_BUFFER,
		_resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());


	const auto maxNumParticles = _resource->getMaxNumParticles();

	const auto numGroupsX = static_cast<UINT>(ceilf(maxNumParticles / 256.0f));
	const auto numGroupsY = 1;
	const auto numGroupsZ = 1;

	for (int sequenceSize = 2; sequenceSize <= maxNumParticles; sequenceSize <<= 1)
	{
		for (int stage = sequenceSize >> 1; stage > 0; stage >>= 1)
		{
			const SortConstants c =
			{
				sequenceSize,
				stage,
			};

			cmdList->SetComputeRoot32BitConstants(ROOT_SLOT_CONSTANTS_BUFFER, sizeof(SortConstants) / 4, &c, 0);

			cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
		}
	}
}

void ParticleSorter::buildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	slotRootParameter[ROOT_SLOT_CONSTANTS_BUFFER]
		.InitAsConstants(sizeof(SortConstants) / 4, 0);
	slotRootParameter[ROOT_SLOT_ALIVE_INDICES_BUFFER]
		.InitAsUnorderedAccessView(0);
	
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

void ParticleSorter::buildShaders()
{
	_shader = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleSortCS.hlsl",
		nullptr,
		"BitonicSortCS",
		"cs_5_1");
}

void ParticleSorter::buildPsos()
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
