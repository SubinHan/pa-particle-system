#include "ParticleResource.h"

#include "Model/Particle.h"
#include "Util/DxDebug.h"
#include "Util/DxUtil.h"
#include "d3dx12.h"

using namespace DirectX;

ParticleResource::ParticleResource(Microsoft::WRL::ComPtr<ID3D12Device> device, ID3D12GraphicsCommandList* cmdList) :
	_device(device)
{
	buildResources(cmdList);
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
	ParticleCounters counters =
	{
		MAX_NUM_PARTICLES,
		0,
		0
	};

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

	//{
	//	const UINT64 deadIndicesByteSize = deadIndices.size() * sizeof(UINT);
	//	const auto buffer =
	//		CD3DX12_RESOURCE_DESC::Buffer(deadIndicesByteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	//	ThrowIfFailed(
	//		_device->CreateCommittedResource(
	//			&heapProperties,
	//			D3D12_HEAP_FLAG_NONE,
	//			&buffer,
	//			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
	//			nullptr,
	//			IID_PPV_ARGS(&_deadIndicesBuffer)
	//		)
	//	);
	//}
	
	{
		const UINT64 countersByteSize = sizeof(ParticleCounters);
		_countersBuffer = DxUtil::createDefaultBuffer(
			_device.Get(),
			cmdList,
			&counters,
			countersByteSize,
			_countersUploadBuffer
		);

		//const auto buffer =
		//	CD3DX12_RESOURCE_DESC::Buffer(countersByteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		//ThrowIfFailed(
		//	_device->CreateCommittedResource(
		//		&heapProperties,
		//		D3D12_HEAP_FLAG_NONE,
		//		&buffer,
		//		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		//		nullptr,
		//		IID_PPV_ARGS(&_countersBuffer)
		//	)
		//);
	}

	const auto deadIndicesToUav = CD3DX12_RESOURCE_BARRIER::Transition(
		_deadIndicesBuffer.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cmdList->ResourceBarrier(1, &deadIndicesToUav);

	const auto countersToUav = CD3DX12_RESOURCE_BARRIER::Transition(
		_countersBuffer.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cmdList->ResourceBarrier(1, &countersToUav);
	
}
