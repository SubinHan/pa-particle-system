#include "Core/ShaderStatementNode/ShaderStatementNodeVortexForce.h"

ShaderStatementNodeVortexForce::ShaderStatementNodeVortexForce(float vortexCenterX, float vortexCenterY, float vortexCenterZ, float vortexAxisX, float vortexAxisY, float vortexAxisZ, float magnitude, float tightness) :
	ShaderStatementNode("UNUSED"),
	_vortexCenterX(vortexCenterX),
	_vortexCenterY(vortexCenterY),
	_vortexCenterZ(vortexCenterZ),
	_vortexAxisX(vortexAxisX),
	_vortexAxisY(vortexAxisY),
	_vortexAxisZ(vortexAxisZ),
	_magnitude(magnitude),
	_tightness(tightness)
{
}

std::string ShaderStatementNodeVortexForce::generateStatements() const
{
	return "currentVelocity += computeVortexForce(currentPosition, float3(" + std::to_string(_vortexCenterX) + ", " + std::to_string(_vortexCenterY) + ", " + std::to_string(_vortexCenterZ) + "), float3(" + std::to_string(_vortexAxisX) + ", " + std::to_string(_vortexAxisY) + ", " + std::to_string(_vortexAxisZ) +"), " + std::to_string(_magnitude) + ", " + std::to_string(_tightness) + ") * DeltaTime;";
}
