#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "Core/ICbvSrvUavDemander.h"

#include <memory>

class ShaderStatementNodeSampleTexture2d : public ShaderStatementNode
{
public:
	ShaderStatementNodeSampleTexture2d(std::string variableName, std::string textureName);
	virtual ~ShaderStatementNodeSampleTexture2d() = default;

	virtual std::string generateStatements() const override;

	virtual int getNumResourcesToBind() const override;
	virtual ResourceViewType getResourceViewType(int index) const override;
	virtual std::string getTypeInShader(int index) const override;
	virtual void onResourceBound(std::vector<std::string> variableNamesInShader) override;
	virtual bool isResourceViewCreated(int index) const override;
	virtual CD3DX12_GPU_DESCRIPTOR_HANDLE getResourceGpuHandle(int index) const override;
private:
	std::string _textureVariableName;
	std::string _textureName;
};