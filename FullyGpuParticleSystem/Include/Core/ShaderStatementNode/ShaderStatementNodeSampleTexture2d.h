#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeSampleTexture2d : public ShaderStatementNode
{
public:
	ShaderStatementNodeSampleTexture2d(std::string variableName);
	virtual ~ShaderStatementNodeSampleTexture2d() = default;

	virtual std::string generateStatements() const override;

private:
};