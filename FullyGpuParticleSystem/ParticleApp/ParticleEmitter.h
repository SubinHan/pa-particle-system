#pragma once

#include "Util/DxUtil.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class ParticleResource;

struct EmitConstants
{
	UINT EmitCount;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Orientation;
	float DeltaTime;
};

class ParticleEmitter
{
public:
	ParticleEmitter(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource);

	ID3D12RootSignature* getRootSignature();
	ID3DBlob* getShader();
	ID3D12PipelineState* getPipelineStateObject();

	void emitParticles(ID3D12GraphicsCommandList* cmdList, int numParticlesToEmit, float deltaTime);

private:
	void buildRootSignature();
	void buildShaders();
	void buildPsos();

private:
	static constexpr int ROOT_SLOT_CONSTANTS_BUFFER = 0;
	static constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_CONSTANTS_BUFFER + 1;
	static constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
	static constexpr int ROOT_SLOT_DEADS_INDICES_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER + 1;
	static constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_DEADS_INDICES_BUFFER + 1;

	Microsoft::WRL::ComPtr<ID3D12Device> _device;
	ParticleResource* _resource;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;
	Microsoft::WRL::ComPtr<ID3DBlob> _shader;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
};