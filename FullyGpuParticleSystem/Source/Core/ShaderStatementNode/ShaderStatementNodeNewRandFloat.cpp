#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat.h"

#include "Util/MathHelper.h"

ShaderStatementNodeNewRandFloat::ShaderStatementNodeNewRandFloat(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeNewRandFloat::generateStatements() const
{
	const float seed = MathHelper::randF();
	return "float " + _variableName +
		" = float(random(" + std::to_string(seed) + " + TotalTime + float(dispatchThreadId.x)));";
}

float ShaderStatementNodeNewRandFloat::getEvaluatedFloat() const
{
	// return average float value
	return 0.5f;
}

float ShaderStatementNodeNewRandFloat::getEvaluatedFloatMin() const
{
	return 0.0f;
}

float ShaderStatementNodeNewRandFloat::getEvaluatedFloatMax() const
{
	return 1.0f;
}
