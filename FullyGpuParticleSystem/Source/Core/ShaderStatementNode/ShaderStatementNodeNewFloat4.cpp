#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat4.h"

ShaderStatementNodeNewFloat4::ShaderStatementNodeNewFloat4(std::string variableName, float x, float y, float z, float w) :
	ShaderStatementNode(variableName),
	_x(x),
	_y(y),
	_z(z),
	_w(w)
{
}

std::string ShaderStatementNodeNewFloat4::generateStatements() const
{
	return "float4 " + _variableName + " = float4(" + 
		std::to_string(_x) + ", " + 
		std::to_string(_y) + ", " + 
		std::to_string(_z) + ", " + 
		std::to_string(_w) + ");";
}
