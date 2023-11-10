#include "Core/HlslGeneratorRender.h"

#include "Core/ShaderStatementGraph.h"
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

Microsoft::WRL::ComPtr<ID3DBlob> HlslGeneratorRender::generateDefaultPixelShader(const std::string psFunctionName)
{
	static const std::wstring BASE_SHADER_PATH = L"ParticleSystemShaders/ParticleRenderBase.hlsl";
	static const std::wstring TEMP_PATH = L"ParticleSystemShaders/Generated/defaultRenderTemp.hlsl";

	HlslGeneratorRender generator(BASE_SHADER_PATH);
	generator.generateShaderFile(TEMP_PATH);
	
	return DxUtil::compileShader(
		TEMP_PATH,
		nullptr,
		psFunctionName,
		"ps_5_1");
}

UINT HlslGeneratorRender::sampleTexture2d(std::string textureName)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeSampleTexture2d>(newLocalVariableName, textureName);
	
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

void HlslGeneratorRender::clip(UINT opacityIndex)
{
	auto newNode =
		std::make_shared<ShaderStatementNodeClip>();
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInputOpacity(_graph->getNode(opacityIndex));
	_graph->linkNode(opacityIndex, nodeIndex);
}

void HlslGeneratorRender::setOutputColor(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetValueByVariableName>("color");
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float4Index));
	_graph->linkNode(float4Index, nodeIndex);
}

UINT HlslGeneratorRender::getParticleColor()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloat3ByVariableName>(newLocalVariableName, "interpolatedColor");
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorRender::getParticleAlpha()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloatByVariableName>(newLocalVariableName, "interpolatedOpacity");
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorRender::sampleTexture2dSubUvAnimation(std::string textureName, float numSubTexturesX, float numSubTexturesY)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeSampleTexture2dSubUvAnimation>(newLocalVariableName, textureName, numSubTexturesX, numSubTexturesY);
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

UINT HlslGeneratorRender::grayscaleToTranslucent(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeGrayscaleToTranslucent>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInputFloat4(_graph->getNode(float4Index));
	_graph->linkNode(float4Index, nodeIndex);

	return nodeIndex;
}
