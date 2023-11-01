#include "Core/HlslGeneratorRender.h"

#include "Core/ShaderStatementNode/ShaderStatementNodeSampleTexture2d.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeClip.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeGrayscaleToTranslucent.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloat3ByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloatByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSampleTexture2dSubUvAnimation.h"

HlslGeneratorRender::HlslGeneratorRender(std::wstring baseShaderPath) :
	HlslGenerator(baseShaderPath)
{
}

HlslGeneratorRender::~HlslGeneratorRender() = default;

UINT HlslGeneratorRender::sampleTexture2d(std::string textureName)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSampleTexture2d>(newLocalVariableName, textureName);
	addNode(newNode);

	return nodeIndex;
}

void HlslGeneratorRender::clip(UINT opacityIndex)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeClip>();
	newNode->setInputOpacity(_nodes[opacityIndex]);
	addNode(newNode);
	linkNode(opacityIndex, nodeIndex);
}

void HlslGeneratorRender::setOutputColor(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("color");
	newNode->setInput(_nodes[float4Index]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);
}

UINT HlslGeneratorRender::getParticleColor()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "interpolatedColor");
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorRender::getParticleAlpha()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloatByVariableName>(newLocalVariableName, "interpolatedOpacity");
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorRender::sampleTexture2dSubUvAnimation(std::string textureName, float numSubTexturesX, float numSubTexturesY)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSampleTexture2dSubUvAnimation>(newLocalVariableName, textureName, numSubTexturesX, numSubTexturesY);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorRender::grayscaleToTranslucent(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGrayscaleToTranslucent>(newLocalVariableName);
	newNode->setInputFloat4(_nodes[float4Index]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);

	return nodeIndex;
}
