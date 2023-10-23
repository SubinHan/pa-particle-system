#include "Core/ShaderStatementNode/ShaderStatementNodeMaskW.h"

ShaderStatementNodeMaskW::ShaderStatementNodeMaskW(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMaskW::generateStatements() const
{
	return "float " + _variableName + " = " + _input->getVariableName() + ".a;";
}

void ShaderStatementNodeMaskW::setInput(std::shared_ptr<ShaderStatementNode> input)
{
	_input = input;
}
