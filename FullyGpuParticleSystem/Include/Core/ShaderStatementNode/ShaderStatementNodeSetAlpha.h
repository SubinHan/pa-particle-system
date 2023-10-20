#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeSetAlpha : public ShaderStatementNode
{
public:
	ShaderStatementNodeSetAlpha(std::string variableName, float value);
	virtual ~ShaderStatementNodeSetAlpha() = default;

	virtual std::string generateStatements() const override;

	void setInput(std::shared_ptr<ShaderStatementNode> input);

private:
	std::shared_ptr<ShaderStatementNode> _input;
	float _value;
};