#pragma once

#include "Core/HlslGenerator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class HlslGeneratorRender : public HlslGenerator
{
public:
	HlslGeneratorRender(std::wstring baseShaderPath);
	virtual ~HlslGeneratorRender();

	UINT sampleTexture2d(std::string textureName);
	void clip(UINT opacityIndex);
	UINT grayscaleToTranslucent(UINT opacityIndex);

	void setOutputColor(UINT float4Index);
	UINT getParticleColor();
	UINT getParticleAlpha();
	UINT sampleTexture2dSubUvAnimation(std::string textureName, float numSubTexturesX, float numSubTexturesY);

private:
};