#pragma once

#include "Core/ParticlePass.h"
#include "Core/Hashable.h"
#include "Util/DxUtil.h"
#include "Model/Geometry.h"
#include "Model/Material.h"

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

class ParticleRenderer : public ParticlePass
{
public:
	ParticleRenderer(ParticleResource* resource, std::string name);

	void setMaterialName(std::string material);

	void render(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb);

	void compileShaders();
	void setShaderPs(Microsoft::WRL::ComPtr<ID3DBlob> shader);

	bool isOpaque();
	void setOpaque(bool newIsOpaque);

protected:
	virtual std::vector<CD3DX12_ROOT_PARAMETER> buildRootParameter() override;
	virtual int getNumSrvUsing() override;
	virtual int getNumUavUsing() override;
	virtual bool needsStaticSampler() override;

	virtual void buildPsos() override;

private:
	void buildCommandSignature();
	void buildDefaultShader();
	void buildInputLayout();
	void generateEmptyGeometry();

	std::string _materialName;

	std::unique_ptr<HlslGeneratorRender> _hlslGenerator;

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

	bool _isOpaque;

	CD3DX12_DESCRIPTOR_RANGE _passCbvTable;
	CD3DX12_DESCRIPTOR_RANGE _texSrvTable;
};