#pragma once

#include "Core/HlslGenerator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class HlslGeneratorSimulate : public HlslGenerator
{
public:
	HlslGeneratorSimulate(std::wstring baseShaderPath);
	virtual ~HlslGeneratorSimulate();

	UINT getPosition();
	UINT getVelocity();
	UINT getAcceleration();

	UINT windForce();
	UINT gravity();
	UINT drag();
	UINT pointAttraction();
	UINT vortex();

	void setPosition(UINT float3Index);
	void setVelocity(UINT float3Index);
	void setAcceleration(UINT float3Index);

private:
};