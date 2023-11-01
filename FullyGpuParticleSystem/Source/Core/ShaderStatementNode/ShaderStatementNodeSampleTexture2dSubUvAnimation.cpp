#include "Core/ShaderStatementNode/ShaderStatementNodeSampleTexture2dSubUvAnimation.h"

#include "Core/TextureManager.h"
#include <assert.h>

ShaderStatementNodeSampleTexture2dSubUvAnimation::ShaderStatementNodeSampleTexture2dSubUvAnimation(
	std::string variableName, 
	std::string textureName,
	int numSubTexturesX,
	int numSubTexturesY) :
	ShaderStatementNode(variableName),
	_textureName(textureName),
	_dimSubTexturesX(numSubTexturesX),
	_dimSubTexturesY(numSubTexturesY)
{
}

std::string ShaderStatementNodeSampleTexture2dSubUvAnimation::generateStatements() const
{
	assert(_dimSubTexturesX != 0);
	assert(_dimSubTexturesY != 0);

	int numSubTextures = _dimSubTexturesX * _dimSubTexturesY;

	std::string actualTexCName = _variableName + "TexC";
	std::string subTextureIndexXName = _variableName + "x";
	std::string subTextureIndexYName = _variableName + "y";

	std::string statement = 
		"uint " + subTextureIndexYName + " = lerp(0.0f, " + std::to_string(numSubTextures) + ", normalizedLifetimeInv) / " + std::to_string(_dimSubTexturesX) + ";\n" +
		"uint " + subTextureIndexXName + " = uint(lerp(0.0f, " + std::to_string(numSubTextures) + ", normalizedLifetimeInv)) % " + std::to_string(_dimSubTexturesX) + ";\n" +
		"float2 " + actualTexCName + " = subUv(pin.TexC, " + std::to_string(_dimSubTexturesX) + ", " + std::to_string(_dimSubTexturesY) + ", " + subTextureIndexXName + ", " + subTextureIndexYName + " );\n" +
		"float4 " + _variableName + " = " + _textureVariableName + ".Sample(gsamLinearWrap, " + actualTexCName + ");\n";

	return statement;
}

int ShaderStatementNodeSampleTexture2dSubUvAnimation::getNumResourcesToBind() const
{
	return 1;
}

ResourceViewType ShaderStatementNodeSampleTexture2dSubUvAnimation::getResourceViewType(int index) const
{
	assert(index == 0 && "index is wrong");
	return ResourceViewType::Srv;
}

std::string ShaderStatementNodeSampleTexture2dSubUvAnimation::getTypeInShader(int index) const
{
	assert(index == 0 && "index is wrong");
	return "Texture2D";
}

void ShaderStatementNodeSampleTexture2dSubUvAnimation::onResourceBound(std::vector<std::string> variableNamesInShader)
{
	assert(variableNamesInShader.size() == 1 && "wrong request submitted.");
	_textureVariableName = variableNamesInShader[0];
}

bool ShaderStatementNodeSampleTexture2dSubUvAnimation::isResourceViewCreated(int index) const
{
	assert(index == 0 && "index is wrong");
	return true;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE ShaderStatementNodeSampleTexture2dSubUvAnimation::getResourceGpuHandle(int index) const
{
	auto textureManager = TextureManager::getInstance();
	return textureManager->getSrvGpuHandle(_textureName);
}
