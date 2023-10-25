#include "Core/HlslTranslatorRender.h"

#include "Core/HlslGeneratorRender.h"
#include "Ui/NodeType.h"

static const std::wstring BASE_RENDERER_SHADER_PATH = L"ParticleSystemShaders/ParticleRenderBase.hlsl";

HlslTranslatorRender::HlslTranslatorRender(std::vector<UiNode> nodes, std::vector<UiLink> links) :
	HlslTranslator(nodes, links)
{
}

HlslTranslatorRender::~HlslTranslatorRender() = default;

std::unique_ptr<HlslGenerator> HlslTranslatorRender::createHlslGenerator()
{
	return std::make_unique<HlslGeneratorRender>(BASE_RENDERER_SHADER_PATH);
}

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslatorRender::compileShaderImpl(std::wstring shaderPath)
{
	return DxUtil::compileShader(
		shaderPath,
		nullptr,
		"ParticlePS",
		"ps_5_1");
}

bool HlslTranslatorRender::translateNode(UiNode node)
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
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));

		UINT input0Index = indexMap[inputNodeId0];

		hlslGeneratorRender->setOutputColor(input0Index);
		break;
	}
	default:
		hasGenerated = false;
		break;
	}

	indexMap[node.getId()] = hlslIndex;
	return hasGenerated;
}
