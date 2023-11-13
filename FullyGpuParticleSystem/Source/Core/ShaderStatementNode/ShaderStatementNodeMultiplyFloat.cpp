#include "Core/ShaderStatementNode/ShaderStatementNodeMultiplyFloat.h"

#include <assert.h>

ShaderStatementNodeMultiplyFloat::ShaderStatementNodeMultiplyFloat(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMultiplyFloat::generateStatements() const
{
	assert(_input0);
	assert(_input1);

	return "float " + _variableName + " = " + _input0->getVariableName() + " * " + _input1->getVariableName() + ";";
}

float ShaderStatementNodeMultiplyFloat::getEvaluatedFloat() const
{
	assert(_input0 && _input1 && "getEvaluatedFloat() called before input set");

	return _input0->getEvaluatedFloat() * _input0->getEvaluatedFloat();
}

void ShaderStatementNodeMultiplyFloat::setInput(std::shared_ptr<ShaderStatementNode> input0, std::shared_ptr<ShaderStatementNode> input1)
{
	_input0 = input0;
	_input1 = input1;
}
