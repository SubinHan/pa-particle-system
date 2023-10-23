#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeMaskX : public ShaderStatementNode
{
public:
	ShaderStatementNodeMaskX(std::string variableName);
	virtual ~ShaderStatementNodeMaskX() = default;

	virtual std::string generateStatements() const override;

	void setInput(std::shared_ptr<ShaderStatementNode> input);

private:
	std::shared_ptr<ShaderStatementNode> _input;
};