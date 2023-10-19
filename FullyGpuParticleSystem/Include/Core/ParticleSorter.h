#pragma once

#include "Util/DxUtil.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class ParticleResource;
struct ObjectConstants;

struct SortConstants
{
	UINT SequenceSize;
	UINT Stage;
};

class ParticleSorter
{
public:
	ParticleSorter(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource);

	ID3D12RootSignature* getRootSignature();
	ID3DBlob* getShader();
	ID3D12PipelineState* getPipelineStateObject();

	void sortParticles(
		ID3D12GraphicsCommandList* cmdList);

private:
	void buildRootSignature();
	void buildShaders();
	void buildPsos();

private:

	Microsoft::WRL::ComPtr<ID3D12Device> _device;
	ParticleResource* _resource;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;
	Microsoft::WRL::ComPtr<ID3DBlob> _shader;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
};