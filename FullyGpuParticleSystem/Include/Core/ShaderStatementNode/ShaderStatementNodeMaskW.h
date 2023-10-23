#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeMaskW : public ShaderStatementNode
{
public:
	ShaderStatementNodeMaskW(std::string variableName);
	virtual ~ShaderStatementNodeMaskW() = default;

	virtual std::string generateStatements() const override;

	void setInput(std::shared_ptr<ShaderStatementNode> input);

private:
	std::shared_ptr<ShaderStatementNode> _input;
};