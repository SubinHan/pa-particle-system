#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeMaskY : public ShaderStatementNode
{
public:
	ShaderStatementNodeMaskY(std::string variableName);
	virtual ~ShaderStatementNodeMaskY() = default;

	virtual std::string generateStatements() const override;

	void setInput(std::shared_ptr<ShaderStatementNode> input);

private:
	std::shared_ptr<ShaderStatementNode> _input;
};