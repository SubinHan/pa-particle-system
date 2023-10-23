#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeClip : public ShaderStatementNode
{
public:
	ShaderStatementNodeClip();
	virtual ~ShaderStatementNodeClip() = default;

	virtual std::string generateStatements() const override;

	void setInputOpacity(
		std::shared_ptr<ShaderStatementNode> inputOpacity);

private:
	std::shared_ptr<ShaderStatementNode> _inputOpacity;
};