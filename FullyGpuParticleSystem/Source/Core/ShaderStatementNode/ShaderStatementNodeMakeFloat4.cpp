#include "Core/ShaderStatementNode/ShaderStatementNodeMakeFloat4.h"

#include <assert.h>

ShaderStatementNodeMakeFloat4::ShaderStatementNodeMakeFloat4(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMakeFloat4::generateStatements() const
{
	assert(_inputX);
	assert(_inputY);
	assert(_inputZ);
	assert(_inputW);
	return "float4 " + _variableName + " = float4(" + _inputX->getVariableName() + ", " + _inputY->getVariableName() + ", " + _inputZ->getVariableName() + ", " + _inputW->getVariableName() + ");";
}

void ShaderStatementNodeMakeFloat4::setInput(
	std::shared_ptr<ShaderStatementNode> inputX, 
	std::shared_ptr<ShaderStatementNode> inputY,
	std::shared_ptr<ShaderStatementNode> inputZ,
	std::shared_ptr<ShaderStatementNode> inputW)
{
	_inputX = inputX;
	_inputY = inputY;
	_inputZ = inputZ;
	_inputW = inputW;
}