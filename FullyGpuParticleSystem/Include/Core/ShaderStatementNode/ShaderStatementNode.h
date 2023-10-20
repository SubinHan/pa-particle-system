#pragma once

#include <string>
#include <vector>
#include <Windows.h>

class ShaderStatementNode
{
public:
	ShaderStatementNode(std::string variableName);
	virtual ~ShaderStatementNode() = default;

	std::string getVariableName() const;
	virtual std::string generateStatements() const = 0;

protected:
	std::string _variableName;
};