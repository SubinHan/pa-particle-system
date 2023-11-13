#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeSetValueByVariableName: public ShaderStatementNode
{
public:
	ShaderStatementNodeSetValueByVariableName(std::string variableNameToSet);
	virtual ~ShaderStatementNodeSetValueByVariableName() = default;

	virtual std::string generateStatements() const override;
	virtual float getEvaluatedFloat() const override;

	void setInput(
		std::shared_ptr<ShaderStatementNode> input);
	std::string getVariableNameToSet();

private:
	std::shared_ptr<ShaderStatementNode> _input;
	std::string _variableNameToSet;
};