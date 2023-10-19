#pragma once

#include "Util/DxUtil.h"
#include "Model/Geometry.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class DxDevice;
class ParticleResource;
class PassConstantBuffer;
struct ObjectConstants;
struct Material;

class ParticlePass
{
public:
	ParticlePass(DxDevice* device, ParticleResource* resource);

	void setMaterial(Material* material);

	void render(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb);

private:
	void buildRootSignature();
	void buildShaders();
	void buildInputLayout();
	void buildPsos();
	void generateEmptyGeometry();

	DxDevice* _device;
	ParticleResource* _resource;
	Material* _material;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderVs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderGs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPs;
	std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;

	std::unique_ptr<MeshGeometry> _emptyGeometry = nullptr;
};