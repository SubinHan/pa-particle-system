#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeEmpty : public ShaderStatementNode
{
public:
	ShaderStatementNodeEmpty();
	virtual ~ShaderStatementNodeEmpty() = default;

	virtual std::string generateStatements() const override;

private:
};