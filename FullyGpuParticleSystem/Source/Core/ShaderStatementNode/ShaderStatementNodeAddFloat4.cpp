#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat4.h"

ShaderStatementNodeAddFloat4::ShaderStatementNodeAddFloat4(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeAddFloat4::generateStatements() const
{
	assert(_input0);
	assert(_input1);

	return "float4 " + _variableName + " = " + _input0->getVariableName() + " + " + _input1->getVariableName() + ";";
}

void ShaderStatementNodeAddFloat4::setInput(std::shared_ptr<ShaderStatementNode> input0, std::shared_ptr<ShaderStatementNode> input1)
{
	_input0 = input0;
	_input1 = input1;
}
