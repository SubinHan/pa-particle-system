#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat3.h"

ShaderStatementNodeNewFloat3::ShaderStatementNodeNewFloat3(std::string variableName, float x, float y, float z) :
	ShaderStatementNode(variableName),
	_x(x),
	_y(y),
	_z(z)
{
}

std::string ShaderStatementNodeNewFloat3::generateStatements() const
{
	return "float3 " + _variableName + " = float3(" +
		std::to_string(_x) + ", " +
		std::to_string(_y) + ", " +
		std::to_string(_z) + ");";
}
