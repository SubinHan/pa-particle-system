#pragma once

#include "Core/ParticleRenderPass.h"
#include "Core/Hashable.h"
#include "Util/DxUtil.h"
#include "Model/Geometry.h"
#include "Model/Material.h"

#include <wrl.h>

enum class RendererType;
enum class RibbonTextureUvType;

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class DxDevice;
class ParticleResource;
class PassConstantBuffer;
class HlslGeneratorRender;
struct ObjectConstants;
struct Material;

struct RibbonDistanceConstants
{
	UINT NumWorkers;
	UINT IndexOffsetFrom;
	UINT IndexOffsetTo;
	UINT ShiftOffset;
};

class ParticleRenderer : public ParticleRenderPass
{
public:
	ParticleRenderer(ParticleResource* resource, std::string name);

	void setMaterialName(std::string material);
	void setRendererType(RendererType type);
	void setRibbonTextureUvType(RibbonTextureUvType type);

	void render(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb);

	void compileShaders();
	void setSpritePixelShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setRibbonPixelShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);

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
	void buildRibbonPso();
	void generateEmptyGeometry();

	void calculateRibbonDistanceFromStart(ID3D12GraphicsCommandList* cmdList);

	ID3D12PipelineState* getCurrentPso();
	ID3D12PipelineState* getCurrentComputePso();
	Microsoft::WRL::ComPtr<ID3DBlob> getCurrentHsRibbon();
	D3D12_PRIMITIVE_TOPOLOGY getCurrentPrimitiveTopology();

	std::unique_ptr<HlslGeneratorRender> _hlslGenerator;

	std::string _materialName;

	// for compute indirect commands.
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _computeRootSignature;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _ribbonDistanceRootSignature;
	Microsoft::WRL::ComPtr<ID3D12CommandSignature> _commandSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> _shaderIndirectCommand;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderIndirectCommandRibbon;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderVs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderGs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderVsRibbon;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderHsRibbon;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderHsRibbonSegmentBased;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderHsRibbonStretched;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderHsRibbonDistanceBased;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderDsRibbon;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderGsRibbon;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPsRibbon;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPreRibbonDistance;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderRibbonDistance;


	std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoOpaque;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoTransparency;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoRibbon;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoComputeIndirect;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoComputeIndirectRibbon;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoPreRibbonDistance;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoRibbonDistance;


	std::unique_ptr<MeshGeometry> _emptyGeometry = nullptr;

	bool _isOpaque;

	CD3DX12_DESCRIPTOR_RANGE _passCbvTable;
	CD3DX12_DESCRIPTOR_RANGE _counterTable;
	CD3DX12_DESCRIPTOR_RANGE _texSrvTable;

	RendererType _currentRendererType;
	RibbonTextureUvType _currentRibbonTextureUvType;
};