#pragma once

#include "Core/HlslTranslator.h"

class ParticleRenderPass;

class HlslTranslatorRenderPs : public HlslTranslator
{
public:
	HlslTranslatorRenderPs(std::vector<UiNode> nodes, std::vector<UiLink> links, ParticleRenderPass* renderer);
	virtual ~HlslTranslatorRenderPs();

protected:
	virtual std::unique_ptr<HlslGenerator> createHlslGenerator() override;
	virtual Microsoft::WRL::ComPtr<ID3DBlob> compileShaderImpl(std::wstring shaderPath) override;

	virtual bool translateNode(UiNode node) override;

private:
	ParticleRenderPass* _renderer;
};