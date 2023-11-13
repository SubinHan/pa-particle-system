#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "..\..\Include\Core\ShaderStatementNode\ShaderStatementNode.h"

#include <d3dx12.h>

ShaderStatementNode::ShaderStatementNode(std::string variableName) :
	_variableName(variableName),
	_evaluatedFloat(0.0f)
{
}

std::string ShaderStatementNode::getVariableName() const
{
	return _variableName;
}