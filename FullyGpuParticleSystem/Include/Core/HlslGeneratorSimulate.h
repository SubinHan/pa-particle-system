#pragma once

#include "Core/HlslGenerator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class HlslGeneratorSimulate : public HlslGenerator
{
public:
	HlslGeneratorSimulate(std::wstring baseShaderPath);
	virtual ~HlslGeneratorSimulate();

	UINT getPositionAfterSimulation();
	UINT getVelcotiyAfterSimulateAcceleration();

	void setPosition(UINT float3Index);
	void setNextFrameVelocity(UINT float3Index);
	void setNextFrameAcceleration(UINT float3Index);

private:
};