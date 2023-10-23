#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeAddFloat3 : public ShaderStatementNode
{
public:
	ShaderStatementNodeAddFloat3(std::string variableName);
	virtual ~ShaderStatementNodeAddFloat3() = default;

	virtual std::string generateStatements() const override;

	void setInput(
		std::shared_ptr<ShaderStatementNode> input0,
		std::shared_ptr<ShaderStatementNode> input1);

private:
	std::shared_ptr<ShaderStatementNode> _input0;
	std::shared_ptr<ShaderStatementNode> _input1;
};