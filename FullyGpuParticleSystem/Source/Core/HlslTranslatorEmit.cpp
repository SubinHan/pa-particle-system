#include "Core/HlslTranslatorEmit.h"

#include "Core/HlslGeneratorEmit.h"
#include "Ui/NodeType.h"

static const std::wstring BASE_EMITTER_SHADER_PATH = L"ParticleSystemShaders/ParticleEmitCSBase.hlsl";

HlslTranslatorEmit::HlslTranslatorEmit(std::vector<UiNode> nodes, std::vector<UiLink> links) :
	HlslTranslator(nodes, links)
{
}

HlslTranslatorEmit::~HlslTranslatorEmit() = default;

std::unique_ptr<HlslGenerator> HlslTranslatorEmit::createHlslGenerator()
{
	return std::make_unique<HlslGeneratorEmit>(BASE_EMITTER_SHADER_PATH);
}

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslatorEmit::compileShaderImpl(std::wstring shaderPath)
{
	return DxUtil::compileShader(
		shaderPath,
		nullptr,
		"EmitCS",
		"cs_5_1");
}

bool HlslTranslatorEmit::translateNode(UiNode node)
{
	if (HlslTranslator::translateNode(node))
		return true;

	bool hasGenerated = true;
	auto nodeType = node.getType();
	UINT hlslIndex{ 0 };

	auto hlslGeneratorEmit = static_cast<HlslGeneratorEmit*>(_hlslGenerator.get());
	assert(hlslGeneratorEmit);

	switch (nodeType)
	{
	case NodeType::EmitterOutput:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));
		const int inputNodeId2 = findOppositeNodeByInputAttrbuteId(node.getInputId(2));
		const int inputNodeId3 = findOppositeNodeByInputAttrbuteId(node.getInputId(3));
		const int inputNodeId4 = findOppositeNodeByInputAttrbuteId(node.getInputId(4));
		const int inputNodeId5 = findOppositeNodeByInputAttrbuteId(node.getInputId(5));

		UINT input0Index = indexMap[inputNodeId0];
		UINT input1Index = indexMap[inputNodeId1];
		UINT input2Index = indexMap[inputNodeId2];
		UINT input3Index = indexMap[inputNodeId3];
		UINT input4Index = indexMap[inputNodeId4];
		UINT input5Index = indexMap[inputNodeId5];

		hlslGeneratorEmit->setInitialPosition(input0Index);
		hlslGeneratorEmit->setInitialVelocity(input1Index);
		hlslGeneratorEmit->setInitialAcceleration(input2Index);
		hlslGeneratorEmit->setInitialLifetime(input3Index);
		hlslGeneratorEmit->setInitialSize(input4Index);
		hlslGeneratorEmit->setInitialOpacity(input5Index);
		break;
	}
	default:
		hasGenerated = false;
		break;
	}

	indexMap[node.getId()] = hlslIndex;
	return hasGenerated;
}
