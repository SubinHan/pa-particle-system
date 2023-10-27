#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <string>

class ShaderStatementNodeWindForce : public ShaderStatementNode
{
public:
	ShaderStatementNodeWindForce(float x, float y, float z);
	virtual ~ShaderStatementNodeWindForce() = default;

	virtual std::string generateStatements() const override;

private:
	float _x;
	float _y;
	float _z;
};