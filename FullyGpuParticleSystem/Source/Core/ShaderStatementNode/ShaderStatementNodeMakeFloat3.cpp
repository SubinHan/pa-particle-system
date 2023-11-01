#include "Core/ShaderStatementNode/ShaderStatementNodeMakeFloat3.h"

#include <assert.h>

ShaderStatementNodeMakeFloat3::ShaderStatementNodeMakeFloat3(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMakeFloat3::generateStatements() const
{
	assert(_inputX);
	assert(_inputY);
	assert(_inputZ);
	return "float3 " + _variableName + " = float3(" + _inputX->getVariableName() + ", " + _inputY->getVariableName() + ", " + _inputZ->getVariableName() + ");";
}

void ShaderStatementNodeMakeFloat3::setInput(
	std::shared_ptr<ShaderStatementNode> inputX,
	std::shared_ptr<ShaderStatementNode> inputY,
	std::shared_ptr<ShaderStatementNode> inputZ)
{
	_inputX = inputX;
	_inputY = inputY;
	_inputZ = inputZ;
}