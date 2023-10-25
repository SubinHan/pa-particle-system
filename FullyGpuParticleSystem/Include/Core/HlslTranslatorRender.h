#pragma once

#include "Core/HlslTranslator.h"

class HlslTranslatorRender : public HlslTranslator
{
public:
	HlslTranslatorRender(std::vector<UiNode> nodes, std::vector<UiLink> links);
	virtual ~HlslTranslatorRender();

protected:
	virtual std::unique_ptr<HlslGenerator> createHlslGenerator() override;
	virtual Microsoft::WRL::ComPtr<ID3DBlob> compileShaderImpl(std::wstring shaderPath) override;

	virtual bool translateNode(UiNode node) override;
};