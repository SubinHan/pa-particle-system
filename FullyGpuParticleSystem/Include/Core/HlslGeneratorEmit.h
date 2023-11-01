#pragma once

#include "Core/HlslGenerator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class HlslGeneratorEmit : public HlslGenerator
{
public:
	HlslGeneratorEmit(std::wstring baseShaderPath);
	virtual ~HlslGeneratorEmit();

	void setInitialPosition(UINT float3Index);
	void setInitialVelocity(UINT float3Index);
	void setInitialAcceleration(UINT float3Index);
	void setInitialLifetime(UINT float1Index);
	void setInitialSize(UINT float1Index);
	void setInitialOpacity(UINT float1Index);
	void setInitialColor(UINT float3Index);
	void setEndSize(UINT float1Index);
	void setEndOpacity(UINT float1Index);
	void setEndColor(UINT float3Index);

private:
};