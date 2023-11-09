#pragma once

#include "Core/ICbvSrvUavDemander.h"

#include <Windows.h>
#include <wrl.h>
#include <d3dx12.h>

struct ParticleCounters
{
	UINT NumAlives;
	UINT NumDeads;
	UINT NumAlivesPostUpdate;
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
	ParticleResource(Microsoft::WRL::ComPtr<ID3D12Device> device, ID3D12GraphicsCommandList* cmdList);

	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;

	void swapAliveIndicesBuffer();

	ID3D12Resource* getParticlesResource();
	ID3D12Resource* getAliveIndicesResourceFront();
	ID3D12Resource* getAliveIndicesResourceBack();
	ID3D12Resource* getDeadIndicesResource();
	ID3D12Resource* getCountersResource();
	ID3D12Resource* getIndirectCommandsResource();
	ID3D12Resource* getIndirectCommandsCounterResetResource();

	UINT getCommandBufferCounterOffset();

	CD3DX12_GPU_DESCRIPTOR_HANDLE getCountersUavGpuHandle();
	CD3DX12_GPU_DESCRIPTOR_HANDLE getIndirectCommandsUavGpuHandle();

	void transitParticlesToSrv(ID3D12GraphicsCommandList* cmdList);
	void transitAliveIndicesToSrv(ID3D12GraphicsCommandList* cmdList);
	void transitParticlesToUav(ID3D12GraphicsCommandList* cmdList);
	void transitAliveIndicesToUav(ID3D12GraphicsCommandList* cmdList);
	void transitCommandBufferToIndirectArgument(ID3D12GraphicsCommandList* cmdList);
	void transitCommandBufferToUav(ID3D12GraphicsCommandList* cmdList);

	int getMaxNumParticles();
	int getReservedParticlesBufferSize();

private:
	void buildResources(ID3D12GraphicsCommandList* cmdList);

private:
	Microsoft::WRL::ComPtr<ID3D12Device> _device;

	Microsoft::WRL::ComPtr<ID3D12Resource> _particlesBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _aliveIndicesBuffer[2]; // for ping-pong
	Microsoft::WRL::ComPtr<ID3D12Resource> _deadIndicesBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _deadIndicesUploadBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _countersBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _countersUploadBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _indirectCommandsBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _indirectCommandsCounterResetBuffer;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> _sortBuffer;

	int _currentAliveIndicesBufferIndex = 0;

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