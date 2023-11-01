#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeNewRandFloat3 : public ShaderStatementNode
{
public:
	ShaderStatementNodeNewRandFloat3(std::string variableName);
	virtual ~ShaderStatementNodeNewRandFloat3() = default;

	virtual std::string generateStatements() const override;

private:
};