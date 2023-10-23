#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeGetFloatByVariableName : public ShaderStatementNode
{
public:
	ShaderStatementNodeGetFloatByVariableName(
		std::string variableName, 
		std::string variableNameToGet);
	virtual ~ShaderStatementNodeGetFloatByVariableName() = default;

	virtual std::string generateStatements() const override;

private:
	std::string _variableNameToGet;
};