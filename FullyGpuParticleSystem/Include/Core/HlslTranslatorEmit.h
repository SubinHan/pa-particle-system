#pragma once

#include "Core/HlslTranslator.h"

class HlslTranslatorEmit : public HlslTranslator
{
public:
	HlslTranslatorEmit(std::vector<UiNode> nodes, std::vector<UiLink> links);
	virtual ~HlslTranslatorEmit();

protected:
	virtual std::unique_ptr<HlslGenerator> createHlslGenerator() override;
	virtual Microsoft::WRL::ComPtr<ID3DBlob> compileShaderImpl(std::wstring shaderPath) override;

	virtual bool generateNode(UiNode node) override;
};