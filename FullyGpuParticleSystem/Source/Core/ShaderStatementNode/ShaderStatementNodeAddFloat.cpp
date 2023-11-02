#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat.h"

#include <assert.h>

ShaderStatementNodeAddFloat::ShaderStatementNodeAddFloat(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeAddFloat::generateStatements() const
{
	assert(_input0);
	assert(_input1);

	return "float " + _variableName + " = " + _input0->getVariableName() + " + " + _input1->getVariableName() + ";";
}

void ShaderStatementNodeAddFloat::setInput(std::shared_ptr<ShaderStatementNode> input0, std::shared_ptr<ShaderStatementNode> input1)
{
	_input0 = input0;
	_input1 = input1;
}
