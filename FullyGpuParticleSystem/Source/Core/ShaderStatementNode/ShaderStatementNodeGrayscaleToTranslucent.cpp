#include "Core/ShaderStatementNode/ShaderStatementNodeGrayscaleToTranslucent.h"

#include <assert.h>

ShaderStatementNodeGrayscaleToTranslucent::ShaderStatementNodeGrayscaleToTranslucent(std::string variableName) :
	ShaderStatementNode(variableName)
{
}

std::string ShaderStatementNodeGrayscaleToTranslucent::generateStatements() const
{
	assert(_inputFloat4);
	return "float4 " + _variableName + " = float4(1.0f, 1.0f, 1.0f, " + _inputFloat4->getVariableName() + ".x);";
}

void ShaderStatementNodeGrayscaleToTranslucent::setInputFloat4(std::shared_ptr<ShaderStatementNode> inputFloat4)
{
	_inputFloat4 = inputFloat4;
}
