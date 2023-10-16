#pragma once

#include "Util/DxUtil.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class DxDevice;
class ParticleResource;

class ParticlePass
{
public:
	ParticlePass(DxDevice* device, ParticleResource* resource);

	void render(ID3D12GraphicsCommandList* cmdList);

private:
	void buildRootSignature();
	void buildShaders();
	void buildInputLayout();
	void buildPsos();

	DxDevice* _device;
	ParticleResource* _resource;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderVs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderGs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPs;
	std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;
};