#include "Core/ShaderStatementNode/ShaderStatementNodeSetColorOfFloat4.h"

ShaderStatementNodeSetColorOfFloat4::ShaderStatementNodeSetColorOfFloat4(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeSetColorOfFloat4::generateStatements() const
{
	return "float4 " + _variableName + " = float4(" + _inputColor->getVariableName() + ".xyz, " + _inputFloat4->getVariableName() + ".a);";
}

void ShaderStatementNodeSetColorOfFloat4::setInputFloat4(std::shared_ptr<ShaderStatementNode> inputFloat4)
{
	_inputFloat4 = inputFloat4;
}

void ShaderStatementNodeSetColorOfFloat4::setInputColor(std::shared_ptr<ShaderStatementNode> inputColor)
{
	_inputColor = inputColor;
}
