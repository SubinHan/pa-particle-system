#include "Core/HlslTranslator.h"

#include "Core/HlslGenerator.h"
#include "Core/ParticleComputePass.h"
#include "Core/ParticleRenderPass.h"
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

	removeOrphanNodes();
	topologySort();
	generateNodes();

	const std::wstring shaderPath = SHADER_ROOT_PATH + std::to_wstring(_hash) + L".hlsl";
	generateShaderFile(shaderPath);

	auto shaderBlob = compileShaderImpl(shaderPath);

	return shaderBlob;
}

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslator::compileShader(ShaderCompileFunction f)
{
	_hlslGenerator = createHlslGenerator();

	removeOrphanNodes();
	topologySort();
	generateNodes();

	const std::wstring shaderPath = SHADER_ROOT_PATH + std::to_wstring(_hash) + L".hlsl";
	generateShaderFile(shaderPath);

	auto shaderBlob = f(shaderPath);

	return shaderBlob;
}

void HlslTranslator::translateTo(ParticleComputePass* pass)
{
	auto blob = compileShader();
	pass->setShaderStatementGraph(_hlslGenerator->getShaderStatementGraph());
	pass->setComputeShader(blob);
}

void HlslTranslator::translateTo(ParticleRenderPass* pass)
{
	auto blob = compileShader();
	pass->setShaderStatementGraph(_hlslGenerator->getShaderStatementGraph());
	pass->setPixelShader(blob);
}

void HlslTranslator::generateNodes()
{
	for (int i = 0; i < _topologicalOrder.size(); ++i)
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
	case NodeType::AddFloat:
	{
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(1));

		if (inputNodeId0 != -1 && inputNodeId1 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			UINT input1Index = indexMap[inputNodeId1];
			hlslIndex = _hlslGenerator->addFloat(input0Index, input1Index);
		}
		break;
	}
	case NodeType::AddFloat3:
	{
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(1));

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
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(1));

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
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(1));
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
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1 )
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex = _hlslGenerator->maskX(input0Index);
		}
		break;
	}
	case NodeType::MaskY:
	{
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex = _hlslGenerator->maskY(input0Index);
		}
		break;
	}
	case NodeType::MaskZ:
	{
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex = _hlslGenerator->maskZ(input0Index);
		}
		break;
	}
	case NodeType::MaskW:
	{
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		if (inputNodeId0 != -1)
		{
			UINT input0Index = indexMap[inputNodeId0];
			hlslIndex = _hlslGenerator->maskW(input0Index);
		}
		break;
	}
	case NodeType::SetColorOfFloat4:
	{
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(1));
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
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(1));
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
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(1));
		const int inputNodeId2 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(2));
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
		const int inputNodeId0 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(0));
		const int inputNodeId1 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(1));
		const int inputNodeId2 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(2));
		const int inputNodeId3 = findOppositeNodeIdByInputAttrbuteId(node.getInputId(3));
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
	case NodeType::SinTime:
	{
		hlslIndex = _hlslGenerator->sinByTime();
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

void HlslTranslator::removeOrphanNodes()
{
	_isOrphan.resize(_nodes.size(), true);
	// IMPORTANT: 0 id node is always output node.
	removeOrphanNodes0(0);

	// find "clip" node because it even doesn't reach result node but not orphan.
	for (int i = 0; i < _nodes.size(); ++i)
	{
		if (_nodes[i].getType() != NodeType::Clip)
			continue;

		removeOrphanNodes0(i);
	}
}

void HlslTranslator::removeOrphanNodes0(const int nodeIndex)
{
	auto node = _nodes[nodeIndex];
	for (int i = 0; i < node.getNumInputs(); ++i)
	{
		const int inputNodeId = findOppositeNodeIdByInputAttrbuteId(node.getInputId(i));

		if (inputNodeId == -1)
			continue;

		int index = getIndex(inputNodeId);

		_isOrphan[index] = false;
		removeOrphanNodes0(index);
	}
}

void HlslTranslator::topologySort()
{
	_visited.resize(_nodes.size());

	for (int i = 0; i < _nodes.size(); ++i)
	{
		if (_visited[i])
			continue;

		if (_isOrphan[i])
			continue;

		topologySort0(i);
	}
}

void HlslTranslator::topologySort0(const int index)
{
	_visited[index] = true;

	for (auto linkedNodeIndex : findLinkedNodesWithOutputOf(index))
	{
		if (_visited[linkedNodeIndex])
			continue;
		topologySort0(linkedNodeIndex);
	}
	_topologicalOrder.push_front(index);
}

int HlslTranslator::getIndex(int nodeId)
{
	for (int i = 0; i < _nodes.size(); ++i)
	{
		if (nodeId == _nodes[i].getId())
			return i;
	}

	return -1;
}

std::vector<int> HlslTranslator::findLinkedNodesWithOutputOf(const int nodeIndex)
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

std::vector<int> HlslTranslator::findLinkedNodesWithInputOf(const int nodeIndex)
{
	std::vector<int> result;

	const int numInputs = _nodes[nodeIndex].getNumInputs();
	
	for (int i = 0; i < numInputs; ++i)
	{
		const int inputId = _nodes[nodeIndex].getInputId(i);

		for (auto link : _links)
		{
			if (link.getFromId() != inputId && link.getToId() != inputId)
				continue;

			const int outputId = link.getOther(inputId);

			for (int i = 0; i < _nodes.size(); ++i)
			{
				if (!_nodes[i].containsAttributeAsOutput(outputId))
					continue;

				result.push_back(i);
				break;
			}
			break;
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

int HlslTranslator::findOppositeNodeIdByInputAttrbuteId(int inputId)
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
