#include "Core/ParticleResource.h"

#include "Model/Particle.h"
#include "Util/DxDebug.h"
#include "Util/DxUtil.h"
#include "d3dx12.h"
#include "d3d11.h"

static constexpr int MAX_NUM_PARTICLES = 50'000;

using namespace DirectX;

ParticleResource::ParticleResource(Microsoft::WRL::ComPtr<ID3D12Device> device, ID3D12GraphicsCommandList* cmdList) :
	_device(device)
{
	buildResources(cmdList);
}

int ParticleResource::getNumDescriptorsToDemand() const
{
	return 2;
}

void ParticleResource::buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu)
{
	_hCounterCpuUav = hCpu;
	_hCounterGpuUav = hGpu;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
	uavDesc.Buffer.NumElements = sizeof(ParticleCounters) / 4;

	_device->CreateUnorderedAccessView(
		_countersBuffer.Get(),
		nullptr,
		&uavDesc,
		_hCounterCpuUav);
}

void ParticleResource::swapAliveIndicesBuffer()
{
	_currentAliveIndicesBufferIndex = (_currentAliveIndicesBufferIndex + 1) % 2;
}

ID3D12Resource* ParticleResource::getParticlesResource()
{
	return _particlesBuffer.Get();
}

ID3D12Resource* ParticleResource::getAliveIndicesResourceFront()
{
	return _aliveIndicesBuffer[_currentAliveIndicesBufferIndex].Get();
}

ID3D12Resource* ParticleResource::getAliveIndicesResourceBack()
{
	const auto other = (_currentAliveIndicesBufferIndex + 1) % 2;
	return _aliveIndicesBuffer[other].Get();
}

ID3D12Resource* ParticleResource::getDeadIndicesResource()
{
	return _deadIndicesBuffer.Get();
}

ID3D12Resource* ParticleResource::getCountersResource()
{
	return _countersBuffer.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ParticleResource::getCountersUavGpuHandle()
{
	return _hCounterGpuUav;
}

void ParticleResource::transitParticlesToSrv(ID3D12GraphicsCommandList* cmdList)
{
	const auto particlesToSrv = CD3DX12_RESOURCE_BARRIER::Transition(
		_particlesBuffer.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_GENERIC_READ);

	cmdList->ResourceBarrier(1, &particlesToSrv);
}

void ParticleResource::transitAliveIndicesToSrv(ID3D12GraphicsCommandList* cmdList)
{
	const auto aliveIndicesToSrv = CD3DX12_RESOURCE_BARRIER::Transition(
		getAliveIndicesResourceFront(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_GENERIC_READ);

	cmdList->ResourceBarrier(1, &aliveIndicesToSrv);
}

void ParticleResource::transitParticlesToUav(ID3D12GraphicsCommandList* cmdList)
{
	const auto particlesToUav = CD3DX12_RESOURCE_BARRIER::Transition(
		_particlesBuffer.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cmdList->ResourceBarrier(1, &particlesToUav);
}

void ParticleResource::transitAliveIndicesToUav(ID3D12GraphicsCommandList* cmdList)
{
	const auto aliveIndicesToUav = CD3DX12_RESOURCE_BARRIER::Transition(
		getAliveIndicesResourceFront(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cmdList->ResourceBarrier(1, &aliveIndicesToUav);
}

int ParticleResource::getMaxNumParticles()
{
	return MAX_NUM_PARTICLES;
}

void ParticleResource::buildResources(ID3D12GraphicsCommandList* cmdList)
{
	std::vector<UINT> deadIndices(MAX_NUM_PARTICLES);
	for (int i = 0; i < MAX_NUM_PARTICLES; ++i)
	{
		deadIndices[i] = i;
	}

	const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	{
		const UINT64 particlesByteSize = MAX_NUM_PARTICLES * sizeof(Particle);
		const auto buffer =
			CD3DX12_RESOURCE_DESC::Buffer(particlesByteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		ThrowIfFailed(
			_device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&buffer,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				nullptr,
				IID_PPV_ARGS(&_particlesBuffer)
			)
		);
	}

	{
		const UINT64 aliveIndicesByteSize = MAX_NUM_PARTICLES * sizeof(UINT);
		const auto buffer =
			CD3DX12_RESOURCE_DESC::Buffer(aliveIndicesByteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		ThrowIfFailed(
			_device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&buffer,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				nullptr,
				IID_PPV_ARGS(&_aliveIndicesBuffer[0])
			)
		);
		ThrowIfFailed(
			_device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&buffer,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				nullptr,
				IID_PPV_ARGS(&_aliveIndicesBuffer[1])
			)
		);
	}

	const UINT64 deadIndicesByteSize = MAX_NUM_PARTICLES * sizeof(UINT);
	_deadIndicesBuffer = DxUtil::createDefaultBuffer(
		_device.Get(),
		cmdList,
		deadIndices.data(),
		deadIndicesByteSize,
		_deadIndicesUploadBuffer
	);
	
	{
		ParticleCounters counters =
		{
			MAX_NUM_PARTICLES,
			0,
			0
		};

		const UINT64 countersByteSize = sizeof(ParticleCounters);

		auto heapPropertiesDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(countersByteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		auto resourceDescUpload = CD3DX12_RESOURCE_DESC::Buffer(countersByteSize);

		// Create the actual default buffer resource.
		ThrowIfFailed(_device->CreateCommittedResource(
			&heapPropertiesDefault,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(_countersBuffer.GetAddressOf())));

		auto heapPropertiesUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		ThrowIfFailed(_device->CreateCommittedResource(
			&heapPropertiesUpload,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescUpload,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(_countersUploadBuffer.GetAddressOf())));


		// Describe the data we want to copy into the default buffer.
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = &counters;
		subResourceData.RowPitch = countersByteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
		// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
		// the intermediate upload heap data will be copied to mBuffer.

		auto barrierCopy = CD3DX12_RESOURCE_BARRIER::Transition(
			_countersBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST
		);

		cmdList->ResourceBarrier(1, &barrierCopy);
		UpdateSubresources<1>(cmdList, _countersBuffer.Get(), _countersUploadBuffer.Get(), 0, 0, 1, &subResourceData);

		auto barrierUpdate = CD3DX12_RESOURCE_BARRIER::Transition(
			_countersBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		cmdList->ResourceBarrier(1, &barrierUpdate);
	}

	const auto deadIndicesToUav = CD3DX12_RESOURCE_BARRIER::Transition(
		_deadIndicesBuffer.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cmdList->ResourceBarrier(1, &deadIndicesToUav);
}
