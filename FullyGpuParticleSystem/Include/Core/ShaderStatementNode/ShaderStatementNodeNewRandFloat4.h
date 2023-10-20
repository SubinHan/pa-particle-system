#pragma once

#include "Util/MathHelper.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeNewRandFloat4 : public ShaderStatementNode
{
public:
	ShaderStatementNodeNewRandFloat4(std::string variableName);
	virtual ~ShaderStatementNodeNewRandFloat4() = default;

	virtual std::string generateStatements() const override;

private:
	float _x;
	float _y;
	float _z;
	float _w;
};