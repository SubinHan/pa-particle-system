#include "Core/ShaderStatementNode/ShaderStatementNodeMaskX.h"

ShaderStatementNodeMaskX::ShaderStatementNodeMaskX(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMaskX::generateStatements() const
{
	return "float " + _variableName + " = " + _input->getVariableName() + ".r;";
}

void ShaderStatementNodeMaskX::setInput(std::shared_ptr<ShaderStatementNode> input)
{
	_input = input;
}
