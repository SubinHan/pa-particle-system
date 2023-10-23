#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeSetAlpha : public ShaderStatementNode
{
public:
	ShaderStatementNodeSetAlpha(std::string variableName);
	virtual ~ShaderStatementNodeSetAlpha() = default;

	virtual std::string generateStatements() const override;

	void setInputFloat4(std::shared_ptr<ShaderStatementNode> inputFloat4);
	void setInputAlpha(std::shared_ptr<ShaderStatementNode> inputAlpha);

private:
	std::shared_ptr<ShaderStatementNode> _inputFloat4;
	std::shared_ptr<ShaderStatementNode> _inputAlpha;
};