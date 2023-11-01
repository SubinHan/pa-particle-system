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
		// initial position
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslGeneratorEmit->setInitialPosition(input0Index);
		}

		// initial velocity
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));
		if (inputNodeId1 != -1)
		{
			UINT input1Index = indexMap[inputNodeId1];
			hlslGeneratorEmit->setInitialVelocity(input1Index);
		}

		// initial acceleration
		const int inputNodeId2 = findOppositeNodeByInputAttrbuteId(node.getInputId(2));
		if (inputNodeId2 != -1)
		{
			UINT input2Index = indexMap[inputNodeId2];
			hlslGeneratorEmit->setInitialAcceleration(input2Index);
		}

		// initial lifetime
		const int inputNodeId3 = findOppositeNodeByInputAttrbuteId(node.getInputId(3));
		if (inputNodeId3 != -1)
		{
			UINT input3Index = indexMap[inputNodeId3];
			hlslGeneratorEmit->setInitialLifetime(input3Index);
		}

		// initial size
		const int inputNodeId4 = findOppositeNodeByInputAttrbuteId(node.getInputId(4));
		if (inputNodeId4 != -1)
		{
			UINT input4Index = indexMap[inputNodeId4];
			hlslGeneratorEmit->setInitialSize(input4Index);
		}

		// initial opacity
		const int inputNodeId5 = findOppositeNodeByInputAttrbuteId(node.getInputId(5));
		if (inputNodeId5 != -1)
		{
			UINT input5Index = indexMap[inputNodeId5];
			hlslGeneratorEmit->setInitialOpacity(input5Index);
		}
		
		// initial color
		const int inputNodeId6 = findOppositeNodeByInputAttrbuteId(node.getInputId(6));
		if (inputNodeId6 != -1)
		{
			UINT initialColorIndex = indexMap[inputNodeId6];
			hlslGeneratorEmit->setInitialColor(initialColorIndex);
		}

		// end size
		const int inputNodeId7 = findOppositeNodeByInputAttrbuteId(node.getInputId(7));
		if (inputNodeId7 != -1)
		{
			UINT endSizeIndex = indexMap[inputNodeId7];
			hlslGeneratorEmit->setEndSize(endSizeIndex);
		}

		// end opacity
		const int inputNodeId8 = findOppositeNodeByInputAttrbuteId(node.getInputId(8));
		if (inputNodeId8 != -1)
		{
			UINT endOpacityIndex = indexMap[inputNodeId8];
			hlslGeneratorEmit->setEndOpacity(endOpacityIndex);
		}
	
		// end color
		const int inputNodeId9 = findOppositeNodeByInputAttrbuteId(node.getInputId(9));
		if (inputNodeId9 != -1)
		{
			UINT endColorIndex = indexMap[inputNodeId9];
			hlslGeneratorEmit->setEndColor(endColorIndex);
		}
		break;
	}
	default:
		hasGenerated = false;
		break;
	}

	indexMap[node.getId()] = hlslIndex;
	return hasGenerated;
}
