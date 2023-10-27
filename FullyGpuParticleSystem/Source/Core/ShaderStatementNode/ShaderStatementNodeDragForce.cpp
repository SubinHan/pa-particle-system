#include "Core/ShaderStatementNode/ShaderStatementNodeDragForce.h"

ShaderStatementNodeDragForce::ShaderStatementNodeDragForce(float dragCoefficient) :
	ShaderStatementNode("UNUSED"),
	_dragCoefficient(dragCoefficient)
{
}

std::string ShaderStatementNodeDragForce::generateStatements() const
{
	return "currentVelocity += computeDragForce(currentVelocity, " + std::to_string(_dragCoefficient) + ");";
}
