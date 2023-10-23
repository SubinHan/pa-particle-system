#pragma once

#include "Util/MathHelper.h"
#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include <memory>

class ShaderStatementNodeMultiplyFloat3ByScalar : public ShaderStatementNode
{
public:
	ShaderStatementNodeMultiplyFloat3ByScalar(std::string variableName);
	virtual ~ShaderStatementNodeMultiplyFloat3ByScalar() = default;

	virtual std::string generateStatements() const override;

	void setInputFloat3(
		std::shared_ptr<ShaderStatementNode> inputFloat3);
	void setInputScalar(
		std::shared_ptr<ShaderStatementNode> inputScalar);

private:
	std::shared_ptr<ShaderStatementNode> _inputFloat3;
	std::shared_ptr<ShaderStatementNode> _inputScalar;
};