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

	CD3DX12_GPU_DESCRIPTOR_HANDLE getCountersUavGpuHandle();

	void transitParticlesToSrv(ID3D12GraphicsCommandList* cmdList);
	void transitAliveIndicesToSrv(ID3D12GraphicsCommandList* cmdList);
	void transitParticlesToUav(ID3D12GraphicsCommandList* cmdList);
	void transitAliveIndicesToUav(ID3D12GraphicsCommandList* cmdList);

	int getMaxNumParticles();

private:
	void buildResources(ID3D12GraphicsCommandList* cmdList);

private:
	static constexpr int NUM_RESOURCES = 5;

	Microsoft::WRL::ComPtr<ID3D12Device> _device;

	Microsoft::WRL::ComPtr<ID3D12Resource> _particlesBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _aliveIndicesBuffer[2]; // for ping-pong
	Microsoft::WRL::ComPtr<ID3D12Resource> _deadIndicesBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _deadIndicesUploadBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> _countersBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> _countersUploadBuffer = nullptr;

	int _currentAliveIndicesBufferIndex = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hCounterCpuUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hCounterGpuUav;
};