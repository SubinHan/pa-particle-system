#include "Core/ParticleResource.h"

#include "Core/DxDevice.h"
#include "Model/Particle.h"
#include "Util/DxDebug.h"
#include "Util/DxUtil.h"
#include "d3dx12.h"
#include "d3d11.h"

// should be power of 2. (for sort)
static constexpr int PARTICLES_BUFFER_SIZE = 1'048'576;
//static constexpr int PARTICLES_BUFFER_SIZE = 65'536;
//static constexpr int PARTICLES_BUFFER_SIZE = 4096;
static constexpr int USABLE_PARTICLES_BUFFER_SIZE = PARTICLES_BUFFER_SIZE - 1;

using namespace DirectX;

std::unique_ptr<ParticleResource> ParticleResource::create(ID3D12GraphicsCommandList* cmdList)
{
	auto created = 
		std::make_unique<ParticleResource>(DxDevice::getInstance().getD3dDevice(), cmdList);

	return std::move(created);
}

ParticleResource::ParticleResource(Microsoft::WRL::ComPtr<ID3D12Device> device, ID3D12GraphicsCommandList* cmdList) :
	_device(device),
	_spawnRatePerSecond(0.0f),
	_averageLifetime(0.0f)
{
	_commandSizePerFrame = getMaxNumParticles() * sizeof(ParticleIndirectCommand);
	_commandBufferCounterOffset = alignForUavCounter(ParticleResource::_commandSizePerFrame);

	buildResources(cmdList);
}

int ParticleResource::getNumDescriptorsToDemand() const
{
	return 3;
}

void ParticleResource::buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu)
{
	_hCounterCpuUav = hCpu;
	_hCounterGpuUav = hGpu;

	{
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

	_hIndirectCommandCpuUav =
		hCpu.Offset(_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	_hIndirectCommandGpuUav =
		hGpu.Offset(_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));


	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = getMaxNumParticles();
		uavDesc.Buffer.StructureByteStride = sizeof(ParticleIndirectCommand);
		uavDesc.Buffer.CounterOffsetInBytes = _commandBufferCounterOffset;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		_device->CreateUnorderedAccessView(
			_indirectCommandsBuffer.Get(),
			_indirectCommandsBuffer.Get(),
			&uavDesc,
			_hIndirectCommandCpuUav);
	}
}

void ParticleResource::swapAliveIndicesBuffer()
{
	_currentParticlesBufferIndex = (_currentParticlesBufferIndex + 1) % 2;
}

ID3D12Resource* ParticleResource::getCurrentParticlesResource()
{
	return _particlesBuffer[_currentParticlesBufferIndex].Get();
}

ID3D12Resource* ParticleResource::getNextParticlesResource()
{
	return _particlesBuffer[(_currentParticlesBufferIndex + 1) % 2].Get();
}

ID3D12Resource* ParticleResource::getCountersResource()
{
	return _countersBuffer.Get();
}

ID3D12Resource* ParticleResource::getCountersTempResource()
{
	return _countersTempBuffer.Get();
}

ID3D12Resource* ParticleResource::getIndirectCommandsResource()
{
	return _indirectCommandsBuffer.Get();
}

ID3D12Resource* ParticleResource::getIndirectCommandsCounterResetResource()
{
	return _indirectCommandsCounterResetBuffer.Get();
}

UINT ParticleResource::getCommandBufferCounterOffset()
{
	return _commandBufferCounterOffset;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ParticleResource::getCountersUavGpuHandle()
{
	return _hCounterGpuUav;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ParticleResource::getIndirectCommandsUavGpuHandle()
{
	return _hIndirectCommandGpuUav;
}

void ParticleResource::onEmittingPolicyChanged(
	float spawnRatePerSecond,
	float averageLifetime,
	float minLifetime,
	float maxLifetime)
{
	_spawnRatePerSecond = spawnRatePerSecond;
	_averageLifetime = averageLifetime;
	_minLifetime = minLifetime;
	_maxLifetime = maxLifetime;
}

UINT ParticleResource::getEstimatedCurrentNumAliveParticles()
{
	return static_cast<UINT>(min(_spawnRatePerSecond * _averageLifetime * 1.5, USABLE_PARTICLES_BUFFER_SIZE));
}

UINT ParticleResource::getEstimatedCurrentNumAliveParticlesAlignedPowerOfTwo()
{
	UINT estimated = getEstimatedCurrentNumAliveParticles();
	UINT result = 1;
	while (result < estimated)
	{
		result <<= 1;
	}

	return result;
}

float ParticleResource::getMinLifetimeOfParticles()
{
	return _minLifetime;
}

float ParticleResource::getMaxLifetimeOfParticles()
{
	return _maxLifetime;
}

void ParticleResource::transitCommandBufferToIndirectArgument(ID3D12GraphicsCommandList* cmdList)
{
	const auto commandBufferToIndirectArgument= CD3DX12_RESOURCE_BARRIER::Transition(
		_indirectCommandsBuffer.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);

	cmdList->ResourceBarrier(1, &commandBufferToIndirectArgument);
}

void ParticleResource::transitCommandBufferToUav(ID3D12GraphicsCommandList* cmdList)
{
	const auto commandBufferToUav = CD3DX12_RESOURCE_BARRIER::Transition(
		_indirectCommandsBuffer.Get(),
		D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cmdList->ResourceBarrier(1, &commandBufferToUav);
}

int ParticleResource::getMaxNumParticles()
{
	// index 0 is reserved. (used for sorting)
	return USABLE_PARTICLES_BUFFER_SIZE;
}

int ParticleResource::getReservedParticlesBufferSize()
{
	// index 0 is reserved. (used for sorting)
	return PARTICLES_BUFFER_SIZE;
}

void ParticleResource::uavBarrier(ID3D12GraphicsCommandList* cmdList)
{
	static const D3D12_RESOURCE_BARRIER barriers[3] =
	{
		CD3DX12_RESOURCE_BARRIER::UAV(getCountersResource()),
		CD3DX12_RESOURCE_BARRIER::UAV(getCurrentParticlesResource()),
		CD3DX12_RESOURCE_BARRIER::UAV(getNextParticlesResource()),
	};

	cmdList->ResourceBarrier(_countof(barriers), barriers);
}

void ParticleResource::transitCountersToCbv(ID3D12GraphicsCommandList* cmdList)
{
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			getCountersResource(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	cmdList->ResourceBarrier(1, &barrier);
}

void ParticleResource::transitCountersCbvToUav(ID3D12GraphicsCommandList* cmdList)
{
	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		getCountersResource(),
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cmdList->ResourceBarrier(1, &barrier);
}

void ParticleResource::buildResources(ID3D12GraphicsCommandList* cmdList)
{
	const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	{
		const UINT64 particlesByteSize = PARTICLES_BUFFER_SIZE * (sizeof(Particle));
		const auto buffer =
			CD3DX12_RESOURCE_DESC::Buffer(particlesByteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		ThrowIfFailed(
			_device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&buffer,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				nullptr,
				IID_PPV_ARGS(&_particlesBuffer[0])
			)
		);
		_particlesBuffer[0]->SetName(L"particlesBuffer0");

		ThrowIfFailed(
			_device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&buffer,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				nullptr,
				IID_PPV_ARGS(&_particlesBuffer[1])
			)
		);
		_particlesBuffer[0]->SetName(L"particlesBuffer1");
	}

	{
		ParticleCounters counters =
		{
			0,
			0,
			0
		};

		const UINT64 countersByteSize = DxUtil::calcConstantBufferByteSize(sizeof(ParticleCounters));

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

		ThrowIfFailed(_device->CreateCommittedResource(
			&heapPropertiesDefault,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(_countersTempBuffer.GetAddressOf())));

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
		subResourceData.RowPitch = sizeof(ParticleCounters);
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
		UpdateSubresources<1>(cmdList, _countersTempBuffer.Get(), _countersUploadBuffer.Get(), 0, 0, 1, &subResourceData);

		auto barrierUpdate = CD3DX12_RESOURCE_BARRIER::Transition(
			_countersBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		cmdList->ResourceBarrier(1, &barrierUpdate);
	}

	{
		auto heapPropertiesDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_commandBufferCounterOffset + sizeof(UINT), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		// Create the actual default buffer resource.
		ThrowIfFailed(_device->CreateCommittedResource(
			&heapPropertiesDefault,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(_indirectCommandsBuffer.GetAddressOf())));
	}

	{
		auto heapPropertiesDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT));

		ThrowIfFailed(_device->CreateCommittedResource(
			&heapPropertiesDefault,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(_indirectCommandsCounterResetBuffer.GetAddressOf())));

		UINT8* mappedCounterReset = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		
		ThrowIfFailed(_indirectCommandsCounterResetBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mappedCounterReset)));
		ZeroMemory(mappedCounterReset, sizeof(UINT));
		_indirectCommandsCounterResetBuffer->Unmap(0, nullptr);
	}

	const auto commandBufferToIndirectArgument = CD3DX12_RESOURCE_BARRIER::Transition(
		_indirectCommandsBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);

	cmdList->ResourceBarrier(1, &commandBufferToIndirectArgument);

	_countersBuffer->SetName(L"Counters");
}
