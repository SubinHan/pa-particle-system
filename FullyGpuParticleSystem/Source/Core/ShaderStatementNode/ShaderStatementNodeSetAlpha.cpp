#include "Core/ShaderStatementNode/ShaderStatementNodeSetAlpha.h"

ShaderStatementNodeSetAlpha::ShaderStatementNodeSetAlpha(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeSetAlpha::generateStatements() const
{
	assert(_inputFloat4);
	assert(_inputAlpha);
	return "float4 " + _variableName + " = float4(" + _inputFloat4->getVariableName() + ".xyz, " + _inputAlpha->getVariableName() + ");";
}

void ShaderStatementNodeSetAlpha::setInputFloat4(std::shared_ptr<ShaderStatementNode> inputFloat4)
{
	_inputFloat4 = inputFloat4;
}

void ShaderStatementNodeSetAlpha::setInputAlpha(std::shared_ptr<ShaderStatementNode> inputAlpha)
{
	_inputAlpha = inputAlpha;
}
