#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat3.h"

ShaderStatementNodeAddFloat3::ShaderStatementNodeAddFloat3(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeAddFloat3::generateStatements() const
{
	assert(_input0);
	assert(_input1);

	return "float3 " + _variableName + " = " + _input0->getVariableName() + " + " + _input1->getVariableName() + ";";
}

void ShaderStatementNodeAddFloat3::setInput(std::shared_ptr<ShaderStatementNode> input0, std::shared_ptr<ShaderStatementNode> input1)
{
	_input0 = input0;
	_input1 = input1;
}
