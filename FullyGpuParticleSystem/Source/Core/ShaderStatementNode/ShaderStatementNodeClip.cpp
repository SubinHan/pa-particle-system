#include "Core/ShaderStatementNode/ShaderStatementNodeClip.h"

ShaderStatementNodeClip::ShaderStatementNodeClip() :
	ShaderStatementNode("UNUSED")
{
}

std::string ShaderStatementNodeClip::generateStatements() const
{
	return "clip(" + _inputOpacity->getVariableName() + ");";
}

void ShaderStatementNodeClip::setInputOpacity(std::shared_ptr<ShaderStatementNode> inputOpacity)
{
	_inputOpacity = inputOpacity;
}