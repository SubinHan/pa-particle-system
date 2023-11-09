#include "Core/ShaderStatementNode/ShaderStatementNodeSinByTime.h"

#include "Util/MathHelper.h"

ShaderStatementNodeSinByTime::ShaderStatementNodeSinByTime(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeSinByTime::generateStatements() const
{
	return "float " + _variableName +
		" = sin(TotalTime);";
}
