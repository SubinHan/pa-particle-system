#include "Core/ShaderStatementNode/ShaderStatementNodeEmpty.h"

ShaderStatementNodeEmpty::ShaderStatementNodeEmpty() :
	ShaderStatementNode("UNUSED")
{
}

std::string ShaderStatementNodeEmpty::generateStatements() const
{
	return std::string();
}
