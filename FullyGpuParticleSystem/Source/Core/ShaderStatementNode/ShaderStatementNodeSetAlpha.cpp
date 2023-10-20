#include "Core/ShaderStatementNode/ShaderStatementNodeSetAlpha.h"

ShaderStatementNodeSetAlpha::ShaderStatementNodeSetAlpha(std::string variableName, float value) :
	ShaderStatementNode(variableName),
	_value(value)
{
}

std::string ShaderStatementNodeSetAlpha::generateStatements() const
{
	assert(_input);
	return "float4 " + _variableName + " = float4(" + _input->getVariableName() + ".xyz, " + std::to_string(_value) + ");";
}

void ShaderStatementNodeSetAlpha::setInput(std::shared_ptr<ShaderStatementNode> input)
{
	_input = input;
}
