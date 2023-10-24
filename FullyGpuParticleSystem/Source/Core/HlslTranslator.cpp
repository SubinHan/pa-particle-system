#include "Core/HlslTranslator.h"

#include "Core/HlslGenerator.h"
#include "Ui/NodeType.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/";

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

	return compileShaderImpl(shaderPath);
}

void HlslTranslator::generateNodes()
{
	for (int i = 0; i < _nodes.size(); ++i)
	{
		generateNode(_nodes[_topologicalOrder[i]]);
	}
}

void HlslTranslator::generateShaderFile(std::wstring shaderPath)
{
	_hlslGenerator->generateShaderFile(shaderPath);
}

bool HlslTranslator::generateNode(UiNode node)
{
	bool hasGenerated = true;
	auto nodeType = node.getType();
	UINT hlslIndex{ 0 };

	switch (nodeType)
	{
	case NodeType::NewFloat:
	{
		const float r = node.getConstantInput(0);
		hlslIndex = _hlslGenerator->newFloat(r);
		break;
	}
	case NodeType::NewFloat3:
	{
		const float r = node.getConstantInput(0);
		const float g = node.getConstantInput(1);
		const float b = node.getConstantInput(2);
		hlslIndex = _hlslGenerator->newFloat3(r, g, b);
		break;
	}
	case NodeType::NewFloat4:
	{
		const float r = node.getConstantInput(0);
		const float g = node.getConstantInput(1);
		const float b = node.getConstantInput(2);
		const float a = node.getConstantInput(3);
		hlslIndex = _hlslGenerator->newFloat4(r, g, b, a);
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

		UINT input0Index = indexMap[inputNodeId0];
		UINT input1Index = indexMap[inputNodeId1];
		hlslIndex = _hlslGenerator->addFloat3(input0Index, input1Index);
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

	// never reach here
	assert(0);
	return -1;
}
