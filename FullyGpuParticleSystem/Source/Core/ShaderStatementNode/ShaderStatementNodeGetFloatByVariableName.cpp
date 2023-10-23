#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloatByVariableName.h"

ShaderStatementNodeGetFloatByVariableName::ShaderStatementNodeGetFloatByVariableName(
	std::string variableName, 
	std::string variableNameToGet) :
	ShaderStatementNode(variableName),
	_variableNameToGet(variableNameToGet)
{
}

std::string ShaderStatementNodeGetFloatByVariableName::generateStatements() const
{
	return "float " + _variableName + " = " + _variableNameToGet + ";";
}
