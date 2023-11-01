#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeMaskZ : public ShaderStatementNode
{
public:
	ShaderStatementNodeMaskZ(std::string variableName);
	virtual ~ShaderStatementNodeMaskZ() = default;

	virtual std::string generateStatements() const override;

	void setInput(std::shared_ptr<ShaderStatementNode> input);

private:
	std::shared_ptr<ShaderStatementNode> _input;
};