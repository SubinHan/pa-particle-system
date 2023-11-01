#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeMakeFloat4 : public ShaderStatementNode
{
public:
	ShaderStatementNodeMakeFloat4(std::string variableName);
	virtual ~ShaderStatementNodeMakeFloat4() = default;

	virtual std::string generateStatements() const override;

	void setInput(
		std::shared_ptr<ShaderStatementNode> inputX, 
		std::shared_ptr<ShaderStatementNode> inputY,
		std::shared_ptr<ShaderStatementNode> inputZ,
		std::shared_ptr<ShaderStatementNode> inputW);

private:
	std::shared_ptr<ShaderStatementNode> _inputX;
	std::shared_ptr<ShaderStatementNode> _inputY;
	std::shared_ptr<ShaderStatementNode> _inputZ;
	std::shared_ptr<ShaderStatementNode> _inputW;
};