#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat.h"

ShaderStatementNodeNewFloat::ShaderStatementNodeNewFloat(std::string variableName, float x) :
	ShaderStatementNode(variableName),
	_x(x)
{
}

std::string ShaderStatementNodeNewFloat::generateStatements() const
{
	return "float " + _variableName + " = float(" +
		std::to_string(_x) + ");";
}

float ShaderStatementNodeNewFloat::getEvaluatedFloat() const
{
	return _x;
}
