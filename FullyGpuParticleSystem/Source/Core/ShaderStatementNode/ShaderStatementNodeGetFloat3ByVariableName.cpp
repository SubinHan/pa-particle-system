#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloat3ByVariableName.h"

ShaderStatementNodeGetFloat3ByVariableName::ShaderStatementNodeGetFloat3ByVariableName(std::string variableName, std::string variableNameToGet) :
	ShaderStatementNode(variableName),
	_variableNameToGet(variableNameToGet)
{
}

std::string ShaderStatementNodeGetFloat3ByVariableName::generateStatements() const
{
	return "float3 " + _variableName + " = " + _variableNameToGet + ";";
}
