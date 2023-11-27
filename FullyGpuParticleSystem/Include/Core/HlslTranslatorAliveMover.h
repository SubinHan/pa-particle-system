#pragma once

#include "Core/HlslTranslatorSimulate.h"

class HlslTranslatorAliveMover : public HlslTranslatorSimulate
{
public:
	HlslTranslatorAliveMover(
		std::vector<UiNode> nodes,
		std::vector<UiLink> links);
	virtual ~HlslTranslatorAliveMover();

protected:
	virtual std::unique_ptr<HlslGenerator> createHlslGenerator() override;
	virtual Microsoft::WRL::ComPtr<ID3DBlob> compileShaderImpl(std::wstring shaderPath) override;
};