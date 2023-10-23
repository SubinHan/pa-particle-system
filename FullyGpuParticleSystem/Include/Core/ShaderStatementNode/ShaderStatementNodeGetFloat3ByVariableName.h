#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeGetFloat3ByVariableName : public ShaderStatementNode
{
public:
	ShaderStatementNodeGetFloat3ByVariableName(
		std::string variableName,
		std::string variableNameToGet);
	virtual ~ShaderStatementNodeGetFloat3ByVariableName() = default;

	virtual std::string generateStatements() const override;

private:
	std::string _variableNameToGet;
};