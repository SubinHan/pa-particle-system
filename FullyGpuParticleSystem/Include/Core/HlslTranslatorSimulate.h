#pragma once

#include "Core/HlslTranslator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class HlslTranslatorSimulate : public HlslTranslator
{
public:
	HlslTranslatorSimulate(std::wstring baseShaderPath);
	virtual ~HlslTranslatorSimulate();

	UINT getPositionAfterSimulation();
	UINT getVelcotiyAfterSimulateAcceleration();

	void setPosition(UINT float3Index);
	void setNextFrameVelocity(UINT float3Index);
	void setNextFrameAcceleration(UINT float3Index);

private:
};