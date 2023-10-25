#pragma once

#include "Core/Hashable.h"
#include "Util/DxUtil.h"
#include "Model/Geometry.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class DxDevice;
class ParticleResource;
class PassConstantBuffer;
class HlslGeneratorRender;
struct ObjectConstants;
struct Material;

class ParticleRenderer : public Hashable
{
public:
	ParticleRenderer(DxDevice* device, ParticleResource* resource, std::string name);

	std::string getName();
	void setMaterial(Material* material);

	void render(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb);

	void compileShaders();
	void setShaderPs(Microsoft::WRL::ComPtr<ID3DBlob> shader);

private:
	void buildRootSignature();
	void buildCommandSignature();
	void buildShaders();
	void buildInputLayout();
	void buildPsos();
	void generateEmptyGeometry();

	DxDevice* _device;
	ParticleResource* _resource;
	Material* _material;

	std::string _name;

	std::unique_ptr<HlslGeneratorRender> _hlslGenerator;

	// for rendering
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
	// for compute indirect commands.
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _computeRootSignature;
	Microsoft::WRL::ComPtr<ID3D12CommandSignature> _commandSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> _shaderIndirectCommand;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderVs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderGs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPs;
	std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoOpaque;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoTransparency;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoCompute;

	std::unique_ptr<MeshGeometry> _emptyGeometry = nullptr;
};