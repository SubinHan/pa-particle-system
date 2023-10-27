#include "Core/ShaderStatementNode/ShaderStatementNodeWindForce.h"

ShaderStatementNodeWindForce::ShaderStatementNodeWindForce(float x, float y, float z)
	: ShaderStatementNode("UNUSED"),
	_x(x),
	_y(y),
	_z(z)
{
}

std::string ShaderStatementNodeWindForce::generateStatements() const
{
	return std::string();
}

