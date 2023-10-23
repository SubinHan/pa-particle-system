#include "Core/ShaderStatementNode/ShaderStatementNodeMultiplyFloat3ByScalar.h"

ShaderStatementNodeMultiplyFloat3ByScalar::ShaderStatementNodeMultiplyFloat3ByScalar(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeMultiplyFloat3ByScalar::generateStatements() const
{
	return "float3 " + _variableName + " = " + _inputFloat3->getVariableName() + " * " + _inputScalar->getVariableName() + ";";
}

void ShaderStatementNodeMultiplyFloat3ByScalar::setInputFloat3(std::shared_ptr<ShaderStatementNode> inputFloat3)
{
	_inputFloat3 = inputFloat3;
}

void ShaderStatementNodeMultiplyFloat3ByScalar::setInputScalar(std::shared_ptr<ShaderStatementNode> inputScalar)
{
	_inputScalar = inputScalar;
}

