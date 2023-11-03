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

struct PreSortConstants
{
	UINT fillEndIndex;
	UINT FillNumber;
};

class ParticleSorter
{
public:
	ParticleSorter(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource, std::string name);

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

	std::string _name;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;
	Microsoft::WRL::ComPtr<ID3DBlob> _shader;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _prePso;
	Microsoft::WRL::ComPtr<ID3DBlob> _preShader;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _preRootSignature;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _prePso0;
	Microsoft::WRL::ComPtr<ID3DBlob> _preShader0;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _preRootSignature0;
};