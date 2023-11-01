#include "Core/HlslTranslator.h"

#include "Core/HlslGenerator.h"
#include "Ui/NodeType.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";

HlslTranslator::HlslTranslator(std::vector<UiNode> nodes, std::vector<UiLink> links) :
	_nodes(nodes),
	_links(links),
	_hlslGenerator(nullptr)
{
}

HlslTranslator::~HlslTranslator() = default;

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslator::compileShader()
{
	_hlslGenerator = createHlslGenerator();

	topologySort();
	generateNodes();

	const std::wstring shaderPath = SHADER_ROOT_PATH + std::to_wstring(_hash) + L".hlsl";
	generateShaderFile(shaderPath);

	auto shaderBlob = compileShaderImpl(shaderPath);

	return shaderBlob;
}

void HlslTranslator::translateTo(ParticlePass* pass)
{
	pass->clearRegisteredShaderStatementNodes();
	auto blob = compileShader();
	registerTranslatedShaderNodesInto(pass);
	pass->setShader(blob);
}

void HlslTranslator::registerTranslatedShaderNodesInto(ParticlePass* pass)
{
	for (auto node : _hlslGenerator->getNodes())
	{
		pass->registerShaderStatementNode(node);
	}
}

void HlslTranslator::generateNodes()
{
	for (int i = 0; i < _nodes.size(); ++i)
	{
		translateNode(_nodes[_topologicalOrder[i]]);
	}
}

void HlslTranslator::generateShaderFile(std::wstring shaderPath)
{
	_hlslGenerator->generateShaderFile(shaderPath);
}

bool HlslTranslator::translateNode(UiNode node)
{
	bool hasGenerated = true;
	auto nodeType = node.getType();
	UINT hlslIndex{ 0 };

	switch (nodeType)
	{
	case NodeType::NewFloat:
	{
		const float r = node.getConstantInputValue(0);
		hlslIndex = _hlslGenerator->newFloat(r);
		break;
	}
	case NodeType::NewFloat3:
	{
		const float r = node.getConstantInputValue(0);
		const float g = node.getConstantInputValue(1);
		const float b = node.getConstantInputValue(2);
		hlslIndex = _hlslGenerator->newFloat3(r, g, b);
		break;
	}
	case NodeType::NewFloat4:
	{
		const float r = node.getConstantInputValue(0);
		const float g = node.getConstantInputValue(1);
		const float b = node.getConstantInputValue(2);
		const float a = node.getConstantInputValue(3);
		hlslIndex = _hlslGenerator->newFloat4(r, g, b, a);
		break;
	}
	case NodeType::RandFloat:
	{
		hlslIndex = _hlslGenerator->randFloat();
		break;
	}
	case NodeType::RandFloat3:
	{
		hlslIndex = _hlslGenerator->randFloat3();
		break;
	}
	case NodeType::AddFloat3:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));

		if (inputNodeId0 != -1 && inputNodeId1 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			UINT input1Index = indexMap[inputNodeId1];
			hlslIndex = _hlslGenerator->addFloat3(input0Index, input1Index);
		}
		break;
	}
	case NodeType::MultiplyFloat:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));

		if (inputNodeId0 != -1 && inputNodeId1 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			UINT input1Index = indexMap[inputNodeId1];
			hlslIndex = _hlslGenerator->multiplyFloat(input0Index, input1Index);
		}
		break;
	}
	case NodeType::MultiplyFloat3ByScalar:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));
		if (inputNodeId0 != -1 && inputNodeId1 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			UINT input1Index = indexMap[inputNodeId1];
			hlslIndex = _hlslGenerator->multiplyFloat3ByScalar(input0Index, input1Index);
		}
		break;
	}
	case NodeType::MaskX:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1 )
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex = _hlslGenerator->maskX(input0Index);
		}
		break;
	}
	case NodeType::MaskY:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex = _hlslGenerator->maskY(input0Index);
		}
		break;
	}
	case NodeType::MaskZ:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex = _hlslGenerator->maskZ(input0Index);
		}
		break;
	}
	case NodeType::MaskW:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex = _hlslGenerator->maskW(input0Index);
		}
		break;
	}
	case NodeType::SetColorOfFloat4:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));
		if (inputNodeId0 != -1 && inputNodeId1 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			UINT input1Index = indexMap[inputNodeId1];
			hlslIndex = _hlslGenerator->setColorOfFloat4(input0Index, input1Index);
		}
		break;
	}
	case NodeType::MakeFloat4ByColorAlpha:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));
		if (inputNodeId0 != -1 && inputNodeId1 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			UINT input1Index = indexMap[inputNodeId1];
			hlslIndex = _hlslGenerator->makeFloat4ByColorAlpha(input0Index, input1Index);
		}
		break;
	}
	case NodeType::MakeFloat3:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));
		const int inputNodeId2 = findOppositeNodeByInputAttrbuteId(node.getInputId(2));
		if (inputNodeId0 != -1 && inputNodeId1 != -1 && inputNodeId2 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			UINT input1Index = indexMap[inputNodeId1];
			UINT input2Index = indexMap[inputNodeId2];
			hlslIndex = _hlslGenerator->makeFloat3(input0Index, input1Index, input2Index);
		}
		break;
	}
	case NodeType::MakeFloat4:
	{
		const int inputNodeId0 = findOppositeNodeByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeByInputAttrbuteId(node.getInputId(1));
		const int inputNodeId2 = findOppositeNodeByInputAttrbuteId(node.getInputId(2));
		const int inputNodeId3 = findOppositeNodeByInputAttrbuteId(node.getInputId(3));
		if (inputNodeId0 != -1 && inputNodeId1 != -1 && inputNodeId2 != -1 && inputNodeId3 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			UINT input1Index = indexMap[inputNodeId1];
			UINT input2Index = indexMap[inputNodeId2];
			UINT input3Index = indexMap[inputNodeId3];
			hlslIndex = _hlslGenerator->makeFloat4(input0Index, input1Index, input2Index, input3Index);
		}
		break;
	}
	default:
		hasGenerated = false;
		break;
	}

	if (hasGenerated)
		indexMap[node.getId()] = hlslIndex;
	
	return hasGenerated;
}

void HlslTranslator::topologySort()
{
	_visited.resize(_nodes.size());

	for (int i = 0; i < _nodes.size(); ++i)
	{
		if (_visited[i])
			continue;

		topologySort0(i);
	}
}

void HlslTranslator::topologySort0(const int index)
{
	_visited[index] = true;

	for (auto linkedNodeIndex : findLinkedNodesWithOutput(index))
	{
		if (_visited[linkedNodeIndex])
			continue;
		topologySort0(linkedNodeIndex);
	}
	_topologicalOrder.push_front(index);
}

std::vector<int> HlslTranslator::findLinkedNodesWithOutput(const int nodeIndex)
{
	std::vector<int> result;

	const int outputId = _nodes[nodeIndex].getOutputId();

	for (auto link : _links)
	{
		if (link.getFromId() != outputId && link.getToId() != outputId)
			continue;

		const int inputId = link.getOther(outputId);

		for (int i = 0; i < _nodes.size(); ++i)
		{
			if (!_nodes[i].containsAttributeAsInput(inputId))
				continue;

			result.push_back(i);
		}
	}

	return result;
}

int HlslTranslator::findNodeIdLinkedAsOutput(UiLink link)
{
	for (auto node : _nodes)
	{
		if (node.getOutputId() == link.getFromId())
			return node.getId();
		if (node.getOutputId() == link.getToId())
			return node.getId();
	}
	// never reach here.
	assert(0);
	return -1;
}

int HlslTranslator::findOppositeNodeByInputAttrbuteId(int inputId)
{
	for (auto link : _links)
	{
		if (link.getFromId() != inputId && link.getToId() != inputId)
			continue;

		const int outputId = link.getOther(inputId);

		for (auto node : _nodes)
		{
			if (node.getOutputId() == outputId)
				return node.getId();
		}
	}

	return -1;
}
