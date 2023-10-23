#pragma once

#include "Core/HlslTranslator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class HlslTranslatorEmit : public HlslTranslator
{
public:
	HlslTranslatorEmit(std::wstring baseShaderPath);
	virtual ~HlslTranslatorEmit();

	void setInitialPosition(UINT float3Index);
	void setInitialVelocity(UINT float3Index);
	void setInitialAcceleration(UINT float3Index);
	void setInitialLifetime(UINT float1Index);
	void setInitialSize(UINT float1Index);
	void setInitialOpacity(UINT float1Index);

private:
};