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

float ShaderStatementNodeSetValueByVariableName::getEvaluatedFloat() const
{
	assert(_input && "getEvaluatedFloat() called before input set");
	return _input->getEvaluatedFloat();
}

float ShaderStatementNodeSetValueByVariableName::getEvaluatedFloatMin() const
{
	assert(_input && "getEvaluatedFloat() called before input set");
	return _input->getEvaluatedFloatMin();;
}

float ShaderStatementNodeSetValueByVariableName::getEvaluatedFloatMax() const
{
	assert(_input && "getEvaluatedFloat() called before input set");
	return _input->getEvaluatedFloatMax();
}

void ShaderStatementNodeSetValueByVariableName::setInput(std::shared_ptr<ShaderStatementNode> input)
{
	_input = input;
}

std::string ShaderStatementNodeSetValueByVariableName::getVariableNameToSet()
{
	return _variableNameToSet;
}
