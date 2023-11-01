#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat3.h"

#include "Util/MathHelper.h"

ShaderStatementNodeNewRandFloat3::ShaderStatementNodeNewRandFloat3(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeNewRandFloat3::generateStatements() const
{
	const float seed = MathHelper::randF();
	const float seedX = seed + 0.1f;
	const float seedY = seed + 0.2f;
	const float seedZ = seed + 0.3f;
	return "float3 " + _variableName + 
		" = float3(random(" + std::to_string(seedX) + " + DeltaTime + float(dispatchThreadId.x)), " +
		"random(" + std::to_string(seedY) + " + DeltaTime + float(dispatchThreadId.x)), " +
		"random(" + std::to_string(seedZ) + " + DeltaTime + float(dispatchThreadId.x)));";
}
