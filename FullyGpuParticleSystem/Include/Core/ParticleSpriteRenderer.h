#pragma once

#include "Core/ParticleRenderPass.h"
#include "Core/PassConstantBuffer.h"
#include "Model/ObjectConstants.h"

enum class RendererType;
enum class RibbonTextureUvType;

class ParticleSpriteRenderer : public ParticleRenderPass
{
public:
	static std::unique_ptr<ParticleSpriteRenderer> create(ParticleResource* resource, std::string name);

	ParticleSpriteRenderer(ParticleResource* resource, std::string name);

	virtual void render(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb) override;

protected:

private:
	void buildShaders();
	void rebuildGraphicsPsos();
	void setShaders();
	void buildComputePsos();

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _spriteOpaquePso;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _spriteTranslucentPso;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _computeIndirectCommandPso;

	RibbonTextureUvType _currentRibbonTextureUvType;

	Microsoft::WRL::ComPtr<ID3DBlob> _vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> _geometryShader;

	Microsoft::WRL::ComPtr<ID3DBlob> _indirectCommandShader;
};