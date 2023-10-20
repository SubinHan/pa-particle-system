#pragma once

#include "Util/MathHelper.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeNewFloat3 : public ShaderStatementNode
{
public:
	ShaderStatementNodeNewFloat3(std::string variableName, float x, float y, float z);
	virtual ~ShaderStatementNodeNewFloat3() = default;

	virtual std::string generateStatements() const override;

private:
	float _x;
	float _y;
	float _z;
};