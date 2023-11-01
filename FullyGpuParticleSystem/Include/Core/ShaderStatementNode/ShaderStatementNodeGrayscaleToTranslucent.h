#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeGrayscaleToTranslucent : public ShaderStatementNode
{
public:
	ShaderStatementNodeGrayscaleToTranslucent(std::string variableName);
	virtual ~ShaderStatementNodeGrayscaleToTranslucent() = default;

	virtual std::string generateStatements() const override;

	void setInputFloat4(std::shared_ptr<ShaderStatementNode> inputFloat4);

private:
	std::shared_ptr<ShaderStatementNode> _inputFloat4;
};