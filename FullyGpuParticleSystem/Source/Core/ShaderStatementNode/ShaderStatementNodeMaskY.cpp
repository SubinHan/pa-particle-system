#include "Core/ShaderStatementNode/ShaderStatementNodeMaskY.h"

ShaderStatementNodeMaskY::ShaderStatementNodeMaskY(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMaskY::generateStatements() const
{
	return "float " + _variableName + " = " + _input->getVariableName() + ".g;";
}

void ShaderStatementNodeMaskY::setInput(std::shared_ptr<ShaderStatementNode> input)
{
	_input = input;
}
