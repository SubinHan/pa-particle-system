#pragma once

#include "Core/ParticleRenderPass.h"
#include "Core/PassConstantBuffer.h"
#include "Model/ObjectConstants.h"

enum class RendererType;
enum class RibbonTextureUvType;

class ParticleRibbonRenderer : public ParticleRenderPass
{
public:
	ParticleRibbonRenderer(ParticleResource* resource, std::string name);

	virtual void render(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb) override;

	void setRibbonTextureUvType(RibbonTextureUvType type);

private:
	void buildShaders();
	void rebuildGraphicsPsos();
	void setShaders();
	void buildComputePsos();

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _ribbonOpaquePso;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _ribbonTranslucentPso;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _computeIndirectCommandPso;

	RibbonTextureUvType _currentRibbonTextureUvType;

	Microsoft::WRL::ComPtr<ID3DBlob> _vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> _domainShader;
	Microsoft::WRL::ComPtr<ID3DBlob> _hullShaderSegmentBased;
	Microsoft::WRL::ComPtr<ID3DBlob> _hullShaderStretched;
	Microsoft::WRL::ComPtr<ID3DBlob> _hullShaderDistanceBased;

	Microsoft::WRL::ComPtr<ID3DBlob> _indirectCommandShader;
};