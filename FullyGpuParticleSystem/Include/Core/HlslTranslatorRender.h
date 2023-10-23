#pragma once

#include "Core/HlslTranslator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class HlslTranslatorRender : public HlslTranslator
{
public:
	HlslTranslatorRender(std::wstring baseShaderPath);
	virtual ~HlslTranslatorRender();

	UINT sampleTexture2d();
	void clip(UINT opacityIndex);

	void setOutputColor(UINT float4Index);

private:
};