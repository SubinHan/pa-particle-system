#pragma once

#include "Util/MathHelper.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeNewFloat4 : public ShaderStatementNode
{
public:
	ShaderStatementNodeNewFloat4(std::string variableName, float x, float y, float z, float w);
	virtual ~ShaderStatementNodeNewFloat4() = default;

	virtual std::string generateStatements() const override;

private:
	float _x;
	float _y;
	float _z;
	float _w;
};