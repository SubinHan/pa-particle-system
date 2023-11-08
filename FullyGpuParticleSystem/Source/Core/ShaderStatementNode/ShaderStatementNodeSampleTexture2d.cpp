#include "Core/ShaderStatementNode/ShaderStatementNodeSampleTexture2d.h"

#include "Core/TextureManager.h"
#include <assert.h>

ShaderStatementNodeSampleTexture2d::ShaderStatementNodeSampleTexture2d(std::string variableName, std::string textureName) :
	ShaderStatementNode(variableName),
	_textureName(textureName)
{
}

std::string ShaderStatementNodeSampleTexture2d::generateStatements() const
{
	return "float4 " + _variableName + " = " + _textureVariableName +".Sample(gsamAnisotropicWrap, pin.TexC);";
}

int ShaderStatementNodeSampleTexture2d::getNumResourcesToBind() const
{
	return 1;
}

ResourceViewType ShaderStatementNodeSampleTexture2d::getResourceViewType(int index) const
{
	assert(index == 0 && "index is wrong");
	return ResourceViewType::Srv;
}

std::string ShaderStatementNodeSampleTexture2d::getTypeInShader(int index) const
{
	assert(index == 0 && "index is wrong");
	return "Texture2D";
}

void ShaderStatementNodeSampleTexture2d::onResourceBound(std::vector<std::string> variableNamesInShader)
{
	assert(variableNamesInShader.size() == 1 && "wrong request submitted.");
	_textureVariableName = variableNamesInShader[0];
}

bool ShaderStatementNodeSampleTexture2d::isResourceViewCreated(int index) const
{
	assert(index == 0 && "index is wrong");
	return true;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ShaderStatementNodeSampleTexture2d::getResourceGpuHandle(int index) const
{
	auto textureManager = TextureManager::getInstance();
	return textureManager->getSrvGpuHandle(_textureName);
}
