#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeSetColorOfFloat4 : public ShaderStatementNode
{
public:
	ShaderStatementNodeSetColorOfFloat4(std::string variableName);
	virtual ~ShaderStatementNodeSetColorOfFloat4() = default;

	virtual std::string generateStatements() const override;

	void setInputFloat4(std::shared_ptr<ShaderStatementNode> inputFloat3OrFloat4);
	void setInputColor(std::shared_ptr<ShaderStatementNode> inputFloat3);

private:
	std::shared_ptr<ShaderStatementNode> _inputFloat4;
	std::shared_ptr<ShaderStatementNode> _inputColor;
};