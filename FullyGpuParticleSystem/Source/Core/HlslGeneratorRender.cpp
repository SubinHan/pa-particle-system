#include "Core/HlslGeneratorRender.h"

#include "Core/ShaderStatementNode/ShaderStatementNodeSampleTexture2d.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeClip.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"

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
