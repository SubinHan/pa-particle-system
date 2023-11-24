#pragma once

#include "Core/ParticleRenderPass.h"
#include "Core/PassConstantBuffer.h"
#include "Model/ObjectConstants.h"

#include <memory>

enum class RendererType;
enum class RibbonTextureUvType;

class ParticlePreDistanceCalculator;
class ParticleDistanceCalculator;

class ParticleRibbonRenderer : public ParticleRenderPass
{
public:
	static std::unique_ptr<ParticleRibbonRenderer> create(ParticleResource* resource, std::string name);

	ParticleRibbonRenderer(ParticleResource* resource, std::string name);
	virtual ~ParticleRibbonRenderer();

	virtual std::string getPixelShaderEntryName() const override;

	virtual void render(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb) override;

	void setRibbonTextureUvType(RibbonTextureUvType type);

protected:
	virtual void updateGeometryShader(bool isBounding) override;

private:
	void buildShaders();
	void rebuildGraphicsPsos();
	void setShaders();
	void buildComputePsos();

private:
	std::unique_ptr<ParticlePreDistanceCalculator> _preDistanceCalculator;
	std::unique_ptr<ParticleDistanceCalculator> _distanceCalculator;

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