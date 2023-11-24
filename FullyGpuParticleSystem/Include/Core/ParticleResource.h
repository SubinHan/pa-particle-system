#pragma once

#include "Core/ICbvSrvUavDemander.h"

#include <memory>
#include <Windows.h>
#include <wrl.h>
#include <d3dx12.h>

struct ParticleCounters
{
	UINT NumAlivesNext;
	UINT NumAlivesCurrent;
	UINT NumSurvived;
};

struct ParticleSortData
{
	UINT ParticleIndex;
	float Distance;
};

struct ParticleIndirectCommand
{
	D3D12_DRAW_INDEXED_ARGUMENTS DrawArguments;
};

class ParticleResource : public ICbvSrvUavDemander
{
public:
	static std::unique_ptr<ParticleResource> create(ID3D12GraphicsCommandList* cmdList);

	ParticleResource(Microsoft::WRL::ComPtr<ID3D12Device> device, ID3D12GraphicsCommandList* cmdList);

	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;

	void swapAliveIndicesBuffer();

	ID3D12Resource* getCurrentParticlesResource();
	ID3D12Resource* getNextParticlesResource();
	ID3D12Resource* getCountersResource();
	ID3D12Resource* getCountersTempResource();
	ID3D12Resource* getIndirectCommandsResource();
	ID3D12Resource* getIndirectCommandsCounterResetResource();

	UINT getCommandBufferCounterOffset();

	CD3DX12_GPU_DESCRIPTOR_HANDLE getCountersUavGpuHandle();
	CD3DX12_GPU_DESCRIPTOR_HANDLE getIndirectCommandsUavGpuHandle();

	void onEmittingPolicyChanged(
		float spawnRatePerSecond,
		float averageLifetime,
		float minLifetime, 
		float maxLifetime);
	UINT getEstimatedCurrentNumAliveParticles();
	UINT getEstimatedCurrentNumAliveParticlesAlignedPowerOfTwo();
	float getMinLifetimeOfParticles();
	float getMaxLifetimeOfParticles();

	void transitCountersToCbv(ID3D12GraphicsCommandList* cmdList);
	void transitCountersCbvToUav(ID3D12GraphicsCommandList* cmdList);
	void transitCommandBufferToIndirectArgument(ID3D12GraphicsCommandList* cmdList);
	void transitCommandBufferToUav(ID3D12GraphicsCommandList* cmdList);

	int getMaxNumParticles();
	int getReservedParticlesBufferSize();

	void uavBarrier(ID3D12GraphicsCommandList* cmdList);

private:
	void buildResources(ID3D12GraphicsCommandList* cmdList);

private:
	Microsoft::WRL::ComPtr<ID3D12Device> _device;

	Microsoft::WRL::ComPtr<ID3D12Resource> _particlesBuffer[2]; // for ping-pong
	Microsoft::WRL::ComPtr<ID3D12Resource> _countersBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _countersUploadBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _indirectCommandsBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _indirectCommandsCounterResetBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _countersTempBuffer;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> _sortBuffer;

	float _spawnRatePerSecond;
	float _averageLifetime;
	float _minLifetime;
	float _maxLifetime;
	int _maxNumParticles;

	int _currentParticlesBufferIndex = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hCounterCpuUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hCounterGpuUav;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hCounterCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hCounterGpuSrv;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hIndirectCommandCpuUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hIndirectCommandGpuUav;

	UINT _commandSizePerFrame;
	UINT _commandBufferCounterOffset;

	static inline UINT alignForUavCounter(UINT bufferSize)
	{
		const UINT alignment = D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT;
		return (bufferSize + (alignment - 1)) & ~(alignment - 1);
	}
};