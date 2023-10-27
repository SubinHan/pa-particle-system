#include "Core/ShaderStatementNode/ShaderStatementNodePointAttractionForce.h"

ShaderStatementNodePointAttractionForce::ShaderStatementNodePointAttractionForce(float x, float y, float z, float radius, float strength) :
	ShaderStatementNode("UNUSED"),
	_x(x),
	_y(y),
	_z(z),
	_radius(radius),
	_strength(strength)
{
}

std::string ShaderStatementNodePointAttractionForce::generateStatements() const
{
	return "currentVelocity += computeAttractionForce(currentPosition, float3(" + std::to_string(_x) + ", " + std::to_string(_y) + ", " + std::to_string(_z) + "), " + std::to_string(_radius) + ", " + std::to_string(_strength) + ") * DeltaTime;";
}
