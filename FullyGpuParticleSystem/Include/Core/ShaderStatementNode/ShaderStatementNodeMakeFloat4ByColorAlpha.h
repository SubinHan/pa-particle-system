#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeMakeFloat4ByColorAlpha : public ShaderStatementNode
{
public:
	ShaderStatementNodeMakeFloat4ByColorAlpha(std::string variableName);
	virtual ~ShaderStatementNodeMakeFloat4ByColorAlpha() = default;

	virtual std::string generateStatements() const override;

	void setInput(std::shared_ptr<ShaderStatementNode> inputColor, std::shared_ptr<ShaderStatementNode> inputAlpha);

private:
	std::shared_ptr<ShaderStatementNode> _inputColor;
	std::shared_ptr<ShaderStatementNode> _inputAlpha;
};