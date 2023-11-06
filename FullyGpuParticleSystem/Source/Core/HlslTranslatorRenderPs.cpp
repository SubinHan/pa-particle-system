#include "Core/HlslTranslatorRenderPs.h"

#include "Core/HlslGeneratorRender.h"
#include "Ui/NodeType.h"

static const std::wstring BASE_RENDERER_SHADER_PATH = L"ParticleSystemShaders/ParticleRenderBase.hlsl";

HlslTranslatorRenderPs::HlslTranslatorRenderPs(std::vector<UiNode> nodes, std::vector<UiLink> links) :
	HlslTranslator(nodes, links)
{
}

HlslTranslatorRenderPs::~HlslTranslatorRenderPs() = default;

std::unique_ptr<HlslGenerator> HlslTranslatorRenderPs::createHlslGenerator()
{
	return std::make_unique<HlslGeneratorRender>(BASE_RENDERER_SHADER_PATH);
}

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslatorRenderPs::compileShaderImpl(std::wstring shaderPath)
{
	return DxUtil::compileShader(
		shaderPath,
		nullptr,
		"ParticlePS",
		"ps_5_1");
}

bool HlslTranslatorRenderPs::translateNode(UiNode node)
{
	if (HlslTranslator::translateNode(node))
		return true;

	bool hasGenerated = true;
	auto nodeType = node.getType();
	UINT hlslIndex{ 0 };

	auto hlslGeneratorRender = static_cast<HlslGeneratorRender*>(_hlslGenerator.get());
	assert(hlslGeneratorRender);

	switch (nodeType)
	{
	case NodeType::RendererOutput:
	{
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslGeneratorRender->setOutputColor(input0Index);
		}
		break;
	}
	case NodeType::SampleTexture2d:
	{
		const std::string textureName = node.getConstantInputValueAsString(0);
		hlslIndex = hlslGeneratorRender->sampleTexture2d(textureName);
		break;
	}
	case NodeType::GrayscaleToTranslucent:
	{
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex =  hlslGeneratorRender->grayscaleToTranslucent(input0Index);
		}
		break;
	}
	case NodeType::GetParticleColor:
	{
		hlslIndex = hlslGeneratorRender->getParticleColor();
		break;
	}
	case NodeType::GetParticleAlpha:
	{
		hlslIndex = hlslGeneratorRender->getParticleAlpha();
		break;
	}
	case NodeType::SampleTexture2dSubUvAnimation:
	{
		const std::string textureName = node.getConstantInputValueAsString(0);
		const float numSubTexturesX = node.getConstantInputValue(1);
		const float numSubTexturesY = node.getConstantInputValue(2);
		hlslIndex = hlslGeneratorRender->sampleTexture2dSubUvAnimation(
			textureName, 
			numSubTexturesX, 
			numSubTexturesY);
		break;
	}
	default:
		hasGenerated = false;
		break;
	}

	indexMap[node.getId()] = hlslIndex;
	return hasGenerated;
}
