#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeMakeFloat3 : public ShaderStatementNode
{
public:
	ShaderStatementNodeMakeFloat3(std::string variableName);
	virtual ~ShaderStatementNodeMakeFloat3() = default;

	virtual std::string generateStatements() const override;

	void setInput(
		std::shared_ptr<ShaderStatementNode> inputX,
		std::shared_ptr<ShaderStatementNode> inputY,
		std::shared_ptr<ShaderStatementNode> inputZ);

private:
	std::shared_ptr<ShaderStatementNode> _inputX;
	std::shared_ptr<ShaderStatementNode> _inputY;
	std::shared_ptr<ShaderStatementNode> _inputZ;
};