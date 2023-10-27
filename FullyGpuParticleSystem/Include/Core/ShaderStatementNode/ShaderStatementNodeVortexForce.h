#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <string>

class ShaderStatementNodeVortexForce : public ShaderStatementNode
{
public:
	ShaderStatementNodeVortexForce(
		float vortexCenterX,
		float vortexCenterY,
		float vortexCenterZ,
		float vortexAxisX,
		float vortexAxisY,
		float vortexAxisZ,
		float magnitude, 
		float tightness);
	virtual ~ShaderStatementNodeVortexForce() = default;

	virtual std::string generateStatements() const override;

private:
	float _vortexCenterX;
	float _vortexCenterY;
	float _vortexCenterZ;
	float _vortexAxisX;
	float _vortexAxisY;
	float _vortexAxisZ;
	float _magnitude;
	float _tightness;
};