#include "Core/ShaderStatementNode/ShaderStatementNodeSampleTexture2d.h"

ShaderStatementNodeSampleTexture2d::ShaderStatementNodeSampleTexture2d(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeSampleTexture2d::generateStatements() const
{
	return "float4 " + _variableName + " = diffuseMap.Sample(gsamPointWrap, pin.TexC);";
}
