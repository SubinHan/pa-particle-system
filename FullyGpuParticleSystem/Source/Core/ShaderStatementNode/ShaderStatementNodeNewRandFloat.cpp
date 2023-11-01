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
		" = float(random(" + std::to_string(seed) + " + DeltaTime + float(dispatchThreadId.x)));";
}
