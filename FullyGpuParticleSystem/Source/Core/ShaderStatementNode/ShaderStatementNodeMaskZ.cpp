#include "Core/ShaderStatementNode/ShaderStatementNodeMaskZ.h"

ShaderStatementNodeMaskZ::ShaderStatementNodeMaskZ(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMaskZ::generateStatements() const
{
	return "float " + _variableName + " = " + _input->getVariableName() + ".b;";
}

void ShaderStatementNodeMaskZ::setInput(std::shared_ptr<ShaderStatementNode> input)
{
	_input = input;
}
