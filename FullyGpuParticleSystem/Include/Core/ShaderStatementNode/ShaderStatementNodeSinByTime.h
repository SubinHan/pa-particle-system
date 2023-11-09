#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeSinByTime : public ShaderStatementNode
{
public:
	ShaderStatementNodeSinByTime(std::string variableName);
	virtual ~ShaderStatementNodeSinByTime() = default;

	virtual std::string generateStatements() const override;

private:
};