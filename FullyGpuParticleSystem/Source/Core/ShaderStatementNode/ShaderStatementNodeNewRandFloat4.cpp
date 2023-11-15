#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat4.h"

ShaderStatementNodeNewRandFloat4::ShaderStatementNodeNewRandFloat4(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeNewRandFloat4::generateStatements() const
{
	const float seed = MathHelper::randF();
	const float seedX = seed + 0.1f;
	const float seedY = seed + 0.2f;
	const float seedZ = seed + 0.3f;
	const float seedW = seed + 0.4f;
	return "float4 " + _variableName + 
		" = float4(random(" + std::to_string(seedX) + " + TotalTime + float(dispatchThreadId.x)), " +
		"random(" + std::to_string(seedY) + " + TotalTime + float(dispatchThreadId.x)), " +
		"random(" + std::to_string(seedZ) + " + TotalTime + float(dispatchThreadId.x)), " +
		"random(" + std::to_string(seedW) + " + TotalTime + float(dispatchThreadId.x)));";
}
