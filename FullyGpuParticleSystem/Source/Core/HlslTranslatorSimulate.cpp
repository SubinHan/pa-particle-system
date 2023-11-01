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

	case NodeType::PointAttractionForce:
	{
		const float x = node.getConstantInputValue(0);
		const float y = node.getConstantInputValue(1);
		const float z = node.getConstantInputValue(2);
		const float radius = node.getConstantInputValue(3);
		const float strength = node.getConstantInputValue(4);

		const int inputNodeId = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId != -1)
		{
			UINT inputIndex = indexMap[inputNodeId];
			hlslIndex = hlslGeneratorSimulate->pointAttraction(inputIndex, x, y, z, radius, strength);
		}
		
		break;
	}

	case NodeType::DragForce:
	{
		const float dragCoefficient = node.getConstantInputValue(0);

		const int inputNodeId = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId != -1)
		{
			UINT inputIndex = indexMap[inputNodeId];
			hlslIndex = hlslGeneratorSimulate->drag(inputIndex, dragCoefficient);
		}
		break;
	}
	
	case NodeType::VortexForce:
	{
		const float vortexCenterX = node.getConstantInputValue(0);
		const float vortexCenterY = node.getConstantInputValue(1);
		const float vortexCenterZ = node.getConstantInputValue(2);
		const float vortexAxisX = node.getConstantInputValue(3);
		const float vortexAxisY = node.getConstantInputValue(4);
		const float vortexAxisZ = node.getConstantInputValue(5);
		const float magnitude = node.getConstantInputValue(6);
		const float tightness = node.getConstantInputValue(7);

		const int inputNodeId = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId != -1)
		{
			UINT inputIndex = indexMap[inputNodeId];
			hlslIndex = hlslGeneratorSimulate->vortex(
				inputIndex,
				vortexCenterX,
				vortexCenterY,
				vortexCenterZ,
				vortexAxisX,
				vortexAxisY,
				vortexAxisZ,
				magnitude,
				tightness);
		}
		break;
	}

	case NodeType::CurlNoiseForce:
	{
		const float amplitude = node.getConstantInputValue(0);
		const float frequency = node.getConstantInputValue(1);

		const int inputNodeId = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId != -1)
		{
			UINT inputIndex = indexMap[inputNodeId];
			hlslIndex = hlslGeneratorSimulate->curlNoise(
				inputIndex,
				amplitude,
				frequency);
		}
		break;
	}

	case NodeType::StartParticleSimulation:
	{
		hlslIndex = hlslGeneratorSimulate->empty();
		break;
	}

	case NodeType::SimulatorOutput:
	{
		break;
	}
	default:
		hasGenerated = false;
		break;
	}

	indexMap[node.getId()] = hlslIndex;
	return hasGenerated;
}
