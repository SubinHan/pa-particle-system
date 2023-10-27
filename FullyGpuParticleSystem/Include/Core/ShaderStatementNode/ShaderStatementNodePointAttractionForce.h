#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <string>

class ShaderStatementNodePointAttractionForce : public ShaderStatementNode
{
public:
	ShaderStatementNodePointAttractionForce(float x, float y, float z, float radius, float strength);
	virtual ~ShaderStatementNodePointAttractionForce() = default;

	virtual std::string generateStatements() const override;

private:
	float _x;
	float _y;
	float _z;
	float _radius;
	float _strength;
};