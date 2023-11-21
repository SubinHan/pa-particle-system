#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeNewRandFloat : public ShaderStatementNode
{
public:
	ShaderStatementNodeNewRandFloat(std::string variableName);
	virtual ~ShaderStatementNodeNewRandFloat() = default;

	virtual std::string generateStatements() const override;
	virtual float getEvaluatedFloat() const override;
	virtual float getEvaluatedFloatMin() const override;
	virtual float getEvaluatedFloatMax() const override;

private:
};