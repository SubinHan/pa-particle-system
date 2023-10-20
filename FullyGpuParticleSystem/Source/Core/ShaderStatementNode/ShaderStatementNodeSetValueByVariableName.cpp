#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"

#include <assert.h>

ShaderStatementNodeSetValueByVariableName::ShaderStatementNodeSetValueByVariableName(std::string variableNameToSet) :
	ShaderStatementNode("UNUSED"),
	_variableNameToSet(variableNameToSet)
{
}

std::string ShaderStatementNodeSetValueByVariableName::generateStatements() const
{
	assert(_input);

	return _variableNameToSet + " = " + _input->getVariableName() + ";";
}

void ShaderStatementNodeSetValueByVariableName::setInput(std::shared_ptr<ShaderStatementNode> input)
{
	_input = input;
}
