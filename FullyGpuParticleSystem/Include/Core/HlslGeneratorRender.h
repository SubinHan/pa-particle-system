#pragma once

#include "Core/HlslGenerator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class HlslGeneratorRender : public HlslGenerator
{
public:
	HlslGeneratorRender(std::wstring baseShaderPath);
	virtual ~HlslGeneratorRender();

	UINT sampleTexture2d();
	void clip(UINT opacityIndex);

	void setOutputColor(UINT float4Index);

private:
};