#include "Core/HlslTranslatorRender.h"

#include "Core/ShaderStatementNode/ShaderStatementNodeSampleTexture2d.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeClip.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"

HlslTranslatorRender::HlslTranslatorRender(std::wstring baseShaderPath) :
	HlslTranslator(baseShaderPath)
{
}

HlslTranslatorRender::~HlslTranslatorRender() = default;

UINT HlslTranslatorRender::sampleTexture2d()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSampleTexture2d>(newLocalVariableName);
	addNode(newNode);

	return nodeIndex;
}

void HlslTranslatorRender::clip(UINT opacityIndex)
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeClip>();
	newNode->setInputOpacity(_nodes[opacityIndex]);
	addNode(newNode);
	linkNode(opacityIndex, nodeIndex);
}

void HlslTranslatorRender::setOutputColor(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("color");
	newNode->setInput(_nodes[float4Index]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);
}
