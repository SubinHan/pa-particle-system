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

	UINT windForce(UINT prerequisite);
	UINT gravity(UINT prerequisite);
	UINT drag(UINT prerequisite, float dragCoefficient);
	UINT pointAttraction(UINT prerequisite, float x, float y, float z, float radius, float strength);
	UINT vortex(
		UINT prerequisite,
		float vortexCenterX,
		float vortexCenterY,
		float vortexCenterZ,
		float vortexAxisX,
		float vortexAxisY,
		float vortexAxisZ,
		float magnitude,
		float tightness);
	UINT curlNoise(
		UINT prerequisite,
		float amplitude,
		float frequency);

	void setPosition(UINT float3Index);
	void setVelocity(UINT float3Index);
	void setAcceleration(UINT float3Index);

private:
};