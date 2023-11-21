#pragma once

#include "Util/MathHelper.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

class ShaderStatementNodeNewFloat : public ShaderStatementNode
{
public:
	ShaderStatementNodeNewFloat(std::string variableName, float x);
	virtual ~ShaderStatementNodeNewFloat() = default;

	virtual std::string generateStatements() const override;
	virtual float getEvaluatedFloat() const override;
	virtual float getEvaluatedFloatMin() const override;
	virtual float getEvaluatedFloatMax() const override;

private:
	float _x;
};