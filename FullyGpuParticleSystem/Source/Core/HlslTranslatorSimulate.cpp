#include "Core/HlslTranslatorSimulate.h"

#include "Core/HlslGeneratorSimulate.h"
#include "Ui/NodeType.h"

static const std::wstring BASE_SIMULATOR_SHADER_PATH = L"ParticleSystemShaders/ParticleSimulateCSBase.hlsl";

HlslTranslatorSimulate::HlslTranslatorSimulate(std::vector<UiNode> nodes, std::vector<UiLink> links) :
	HlslTranslator(nodes, links)
{
}

HlslTranslatorSimulate::~HlslTranslatorSimulate() = default;

std::unique_ptr<HlslGenerator> HlslTranslatorSimulate::createHlslGenerator()
{
	return std::make_unique<HlslGeneratorSimulate>(BASE_SIMULATOR_SHADER_PATH);
}

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslatorSimulate::compileShaderImpl(std::wstring shaderPath)
{
	return DxUtil::compileShader(
		shaderPath,
		nullptr,
		"SimulateCS",
		"cs_5_1");
}

bool HlslTranslatorSimulate::translateNode(UiNode node)
{
	if (HlslTranslator::translateNode(node))
		return true;

	bool hasGenerated = true;
	auto nodeType = node.getType();
	UINT hlslIndex{ 0 };

	auto hlslGeneratorSimulate = static_cast<HlslGeneratorSimulate*>(_hlslGenerator.get());
	assert(hlslGeneratorSimulate);

	switch (nodeType)
	{
	case NodeType::GetParticlePosition:
		hlslIndex = hlslGeneratorSimulate->getPosition();
		break;

	case NodeType::GetParticleVelocity:
		hlslIndex = hlslGeneratorSimulate->getVelocity();
		break;

	case NodeType::GetParticleAcceleration:
		hlslIndex = hlslGeneratorSimulate->getAcceleration();
		break;

	case NodeType::SimulatorOutput:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));
		const int inputNodeId2 = findOppositeNodeByInputAttrbuteId(node.getInputId(2));

		UINT input0Index = indexMap[inputNodeId0];
		UINT input1Index = indexMap[inputNodeId1];
		UINT input2Index = indexMap[inputNodeId2];

		hlslGeneratorSimulate->setPosition(input0Index);
		hlslGeneratorSimulate->setVelocity(input1Index);
		hlslGeneratorSimulate->setAcceleration(input2Index);
		break;
	}
	default:
		hasGenerated = false;
		break;
	}

	indexMap[node.getId()] = hlslIndex;
	return hasGenerated;
}
