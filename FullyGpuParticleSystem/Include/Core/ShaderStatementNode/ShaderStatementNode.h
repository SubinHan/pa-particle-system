#pragma once

#include "Model/ResourceRequest.h"

#include <string>
#include <vector>
#include <Windows.h>
#include <assert.h>

struct ID3D12Resource;

class ShaderStatementNode
{
public:
	ShaderStatementNode(std::string variableName);
	virtual ~ShaderStatementNode() = default;

	std::string getVariableName() const;
	virtual std::string generateStatements() const = 0;

	virtual int getNumResourcesToBind() const { return 0; }

	virtual ResourceViewType getResourceViewType(int index) const { return ResourceViewType::Invalid; }
	virtual std::string getTypeInShader(int index) const { return std::string(); }
	virtual void onResourceBound(std::vector<std::string> variableNamesInShader) {}

	virtual bool isResourceViewCreated(int index) const { return false; }
	virtual CD3DX12_GPU_DESCRIPTOR_HANDLE getResourceGpuHandle(int index) const { return CD3DX12_GPU_DESCRIPTOR_HANDLE{}; }

	virtual float getEvaluatedFloat() const { assert(0 && "Calling getEvaluatedFloat() is not expected on this node.");  return 0.0f; };

protected:
	std::string _variableName;
	float _evaluatedFloat;
};