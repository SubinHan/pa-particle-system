#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeAddFloat : public ShaderStatementNode
{
public:
	ShaderStatementNodeAddFloat(std::string variableName);
	virtual ~ShaderStatementNodeAddFloat() = default;

	virtual std::string generateStatements() const override;

	void setInput(
		std::shared_ptr<ShaderStatementNode> input0,
		std::shared_ptr<ShaderStatementNode> input1);

private:
	std::shared_ptr<ShaderStatementNode> _input0;
	std::shared_ptr<ShaderStatementNode> _input1;
};