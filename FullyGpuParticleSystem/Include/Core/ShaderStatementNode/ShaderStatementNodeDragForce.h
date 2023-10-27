#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <string>

class ShaderStatementNodeDragForce : public ShaderStatementNode
{
public:
	ShaderStatementNodeDragForce(float dragCoefficient);
	virtual ~ShaderStatementNodeDragForce() = default;

	virtual std::string generateStatements() const override;

private:
	float _dragCoefficient;
};