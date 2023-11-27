#pragma once

#include "Core/HlslTranslatorSimulate.h"

class HlslTranslatorDestroyer : public HlslTranslatorSimulate
{
public:
	HlslTranslatorDestroyer(
		std::vector<UiNode> nodes,
		std::vector<UiLink> links);
	virtual ~HlslTranslatorDestroyer();

protected:
	virtual std::unique_ptr<HlslGenerator> createHlslGenerator() override;
	virtual Microsoft::WRL::ComPtr<ID3DBlob> compileShaderImpl(std::wstring shaderPath) override;
};