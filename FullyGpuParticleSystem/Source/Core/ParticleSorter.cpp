#include "Core/ParticleSorter.h"

#include "Core/ParticleResource.h"

using namespace Microsoft::WRL;

constexpr int SPAWN_ROOT_SLOT_CONSTANTS_BUFFER = 0;
constexpr int SPAWN_ROOT_SLOT_PARTICLES_BUFFER = SPAWN_ROOT_SLOT_CONSTANTS_BUFFER + 1;
constexpr int SPAWN_ROOT_SLOT_ALIVE_INDICES_BUFFER = SPAWN_ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int SPAWN_ROOT_SLOT_COUNTERS_BUFFER = SPAWN_ROOT_SLOT_ALIVE_INDICES_BUFFER + 1;

constexpr int PRE_ROOT_SLOT_CONSTANTS_BUFFER = 0;
constexpr int PRE_ROOT_SLOT_ALIVE_INDICES_BUFFER = PRE_ROOT_SLOT_CONSTANTS_BUFFER + 1;
constexpr int PRE_ROOT_SLOT_COUNTERS_BUFFER = PRE_ROOT_SLOT_ALIVE_INDICES_BUFFER + 1;


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
	return _shaderBySpawnOrder.Get();
}

ID3D12PipelineState* ParticleSorter::getPipelineStateObject()
{
	return _psoBySpawnOrder.Get();
}

void ParticleSorter::sortParticles(ID3D12GraphicsCommandList* cmdList)
{
	{
		// pre sort
		cmdList->SetComputeRootSignature(_preRootSignature.Get());
		cmdList->SetPipelineState(_prePsoBySpawnOrder.Get());

		const PreSortConstants c =
		{
			_resource->getReservedParticlesBufferSize(),
			0,
		};

		cmdList->SetComputeRoot32BitConstants(SPAWN_ROOT_SLOT_CONSTANTS_BUFFER, sizeof(PreSortConstants) / 4, &c, 0);

		cmdList->SetComputeRootUnorderedAccessView(
			SPAWN_ROOT_SLOT_PARTICLES_BUFFER,
			_resource->getParticlesResource()->GetGPUVirtualAddress());

		cmdList->SetComputeRootUnorderedAccessView(
			SPAWN_ROOT_SLOT_ALIVE_INDICES_BUFFER,
			_resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());

		cmdList->SetComputeRootDescriptorTable(
			SPAWN_ROOT_SLOT_COUNTERS_BUFFER,
			_resource->getCountersUavGpuHandle());

		const auto maxNumParticles = _resource->getMaxNumParticles();

		const auto numGroupsX = static_cast<UINT>(ceilf(maxNumParticles / 256.0f));
		const auto numGroupsY = 1;
		const auto numGroupsZ = 1;

		cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
	}

	{
		// sort
		cmdList->SetComputeRootSignature(_rootSignature.Get());
		cmdList->SetPipelineState(_psoBySpawnOrder.Get());

		cmdList->SetComputeRootUnorderedAccessView(
			SPAWN_ROOT_SLOT_PARTICLES_BUFFER,
			_resource->getParticlesResource()->GetGPUVirtualAddress());

		cmdList->SetComputeRootUnorderedAccessView(
			SPAWN_ROOT_SLOT_ALIVE_INDICES_BUFFER,
			_resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());

		cmdList->SetComputeRootDescriptorTable(
			SPAWN_ROOT_SLOT_COUNTERS_BUFFER,
			_resource->getCountersUavGpuHandle());

		const auto maxNumParticles = _resource->getReservedParticlesBufferSize();

		const auto numGroupsX = static_cast<UINT>(ceilf(maxNumParticles / 256.0f));
		const auto numGroupsY = 1;
		const auto numGroupsZ = 1;

		for (int sequenceSize = 2; sequenceSize <= maxNumParticles; sequenceSize <<= 1)
		{
			for (int stage = sequenceSize >> 1; stage > 0; stage >>= 1)
			{
				const SortConstants c =
				{
					maxNumParticles,
					sequenceSize,
					stage,
				};

				cmdList->SetComputeRoot32BitConstants(SPAWN_ROOT_SLOT_CONSTANTS_BUFFER, sizeof(SortConstants) / 4, &c, 0);

				cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
			}
		}
	}
}

void ParticleSorter::buildRootSignature()
{
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		slotRootParameter[SPAWN_ROOT_SLOT_CONSTANTS_BUFFER]
			.InitAsConstants(sizeof(SortConstants) / 4, 0);
		slotRootParameter[SPAWN_ROOT_SLOT_PARTICLES_BUFFER]
			.InitAsUnorderedAccessView(0);
		slotRootParameter[SPAWN_ROOT_SLOT_ALIVE_INDICES_BUFFER]
			.InitAsUnorderedAccessView(1);

		CD3DX12_DESCRIPTOR_RANGE uavTable;
		uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
		slotRootParameter[SPAWN_ROOT_SLOT_COUNTERS_BUFFER]
			.InitAsDescriptorTable(1, &uavTable);

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

	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		slotRootParameter[SPAWN_ROOT_SLOT_CONSTANTS_BUFFER]
			.InitAsConstants(sizeof(SortConstants) / 4, 0);
		slotRootParameter[SPAWN_ROOT_SLOT_PARTICLES_BUFFER]
			.InitAsUnorderedAccessView(0);
		slotRootParameter[SPAWN_ROOT_SLOT_ALIVE_INDICES_BUFFER]
			.InitAsUnorderedAccessView(1);

		CD3DX12_DESCRIPTOR_RANGE uavTable;
		uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
		slotRootParameter[SPAWN_ROOT_SLOT_COUNTERS_BUFFER]
			.InitAsDescriptorTable(1, &uavTable);

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
			IID_PPV_ARGS(_preRootSignature.GetAddressOf())));
	}
}

void ParticleSorter::buildShaders()
{
	_shaderByIndex = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleSortByIndexCS.hlsl",
		nullptr,
		"BitonicSortCS",
		"cs_5_1");

	_shaderBySpawnOrder = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleSortBySpawnOrderCS.hlsl",
		nullptr,
		"BitonicSortCS",
		"cs_5_1");

	_preShaderByIndex = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticlePreSortCS.hlsl",
		nullptr,
		"PreSortCS",
		"cs_5_1");

	_preShaderBySpawnOrder = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticlePreSortCS0.hlsl",
		nullptr,
		"PreSortCS",
		"cs_5_1");
}

void ParticleSorter::buildPsos()
{
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = _rootSignature.Get();
		psoDesc.CS =
		{
			reinterpret_cast<BYTE*>(_shaderBySpawnOrder->GetBufferPointer()),
			_shaderBySpawnOrder->GetBufferSize()
		};
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(
			_device->CreateComputePipelineState(
				&psoDesc, IID_PPV_ARGS(&_psoBySpawnOrder)));
	}

	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = _rootSignature.Get();
		psoDesc.CS =
		{
			reinterpret_cast<BYTE*>(_shaderByIndex->GetBufferPointer()),
			_shaderByIndex->GetBufferSize()
		};
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(
			_device->CreateComputePipelineState(
				&psoDesc, IID_PPV_ARGS(&_psoByIndex)));
	}

	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = _preRootSignature.Get();
		psoDesc.CS =
		{
			reinterpret_cast<BYTE*>(_preShaderByIndex->GetBufferPointer()),
			_preShaderByIndex->GetBufferSize()
		};
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(
			_device->CreateComputePipelineState(
				&psoDesc, IID_PPV_ARGS(&_prePsoByIndex)));
	}

	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = _preRootSignature.Get();
		psoDesc.CS =
		{
			reinterpret_cast<BYTE*>(_preShaderBySpawnOrder->GetBufferPointer()),
			_preShaderBySpawnOrder->GetBufferSize()
		};
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(
			_device->CreateComputePipelineState(
				&psoDesc, IID_PPV_ARGS(&_prePsoBySpawnOrder)));
	}
}
