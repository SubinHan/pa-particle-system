#include "Core/ShaderStatementNode/ShaderStatementNodeMakeFloat4ByColorAlpha.h"

#include <assert.h>

ShaderStatementNodeMakeFloat4ByColorAlpha::ShaderStatementNodeMakeFloat4ByColorAlpha(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMakeFloat4ByColorAlpha::generateStatements() const
{
	assert(_inputColor);
	assert(_inputAlpha);
	return "float4 " + _variableName + " = float4(" + _inputColor->getVariableName() + ", " + _inputAlpha->getVariableName() + ");";
}

void ShaderStatementNodeMakeFloat4ByColorAlpha::setInput(std::shared_ptr<ShaderStatementNode> inputColor, std::shared_ptr<ShaderStatementNode> inputAlpha)
{
	_inputColor = inputColor;
	_inputAlpha = inputAlpha;
}
