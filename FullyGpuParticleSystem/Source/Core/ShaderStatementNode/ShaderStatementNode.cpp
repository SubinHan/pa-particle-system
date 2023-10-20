#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "..\..\Include\Core\ShaderStatementNode\ShaderStatementNode.h"

ShaderStatementNode::ShaderStatementNode(std::string variableName) :
	_variableName(variableName)
{
}

std::string ShaderStatementNode::getVariableName() const
{
	return _variableName;
}
