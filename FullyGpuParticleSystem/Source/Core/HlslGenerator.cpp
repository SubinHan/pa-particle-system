#include "Core/HlslGenerator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeEmpty.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetAlpha.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMultiplyFloat.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMultiplyFloat3ByScalar.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMaskX.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMaskY.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMaskZ.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMaskW.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloatByVariableName.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetColorOfFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMakeFloat4ByColorAlpha.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMakeFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMakeFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSinByTime.h"
#include "Model/ResourceViewType.h"
#include "Util/DxUtil.h"

#include <fstream>
#include <assert.h>
#include <queue>

static const std::wstring SHADER_TEMP_PATH = L"ParticleSystemShaders/Generated/temp.hlsl";

HlslGenerator::HlslGenerator(std::wstring baseShaderPath) :
	_baseShaderPath(baseShaderPath),
	_numSrvInBaseShader(0),
	_numUavInBaseShader(0)
{
	findNumRegisters();
}

HlslGenerator::~HlslGenerator() = default;

void HlslGenerator::generateShaderFile(const std::wstring& outputPath)
{
	//std::vector<ResourceRequest> srvRequests = collectSrvRequests();

	std::ifstream fin;
	fin.open(_baseShaderPath);
	assert(fin.is_open());

	std::ofstream fout;
	fout.open(outputPath);
	assert(fout.is_open());

	// topology sort
	const UINT numNodes = _graph.size();
	_visited.resize(numNodes);
	std::fill(_visited.begin(), _visited.end(), false);

	for (int i = 0; i < numNodes; ++i)
	{
		if (_visited[i])
			continue;

		topologySort(i);
	}

	constexpr UINT BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];

	while (fin.getline(buffer, BUFFER_SIZE))
	{
		std::string line(buffer);

		if (line.find("%s") != std::string::npos)
		{
			insertStatements(fout);
			continue;
		}
		
		if (line.find("%u") != std::string::npos)
		{
			// TODO: insert unordered access view
			continue;
		}

		if (line.find("%t") != std::string::npos)
		{
			insertSrvs(fout);
			continue;
		}

		fout << buffer << std::endl;
	}

	fin.close();
	fout.close();
}

std::vector<std::shared_ptr<ShaderStatementNode>> HlslGenerator::getNodes()
{
	return _nodes;
}

UINT HlslGenerator::empty()
{
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeEmpty>();
	addNode(newNode);
	return nodeIndex;
}

UINT HlslGenerator::newFloat(float x)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewFloat>(newLocalVariableName, x);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::newFloat3(float x, float y, float z)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewFloat3>(newLocalVariableName, x, y, z);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::newFloat4(float x, float y, float z, float w)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode = 
		std::make_shared<ShaderStatementNodeNewFloat4>(newLocalVariableName, x, y, z, w);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::randFloat()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewRandFloat>(newLocalVariableName);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::randFloat3()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewRandFloat3>(newLocalVariableName);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::randFloat4()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewRandFloat4>(newLocalVariableName);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::setAlpha(UINT float4Index, UINT alphaIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetAlpha>(newLocalVariableName);
	newNode->setInputFloat4(_nodes[float4Index]);
	newNode->setInputAlpha(_nodes[alphaIndex]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);
	linkNode(alphaIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::addFloat4(UINT float4Index0, UINT float4Index1)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeAddFloat4>(newLocalVariableName);
	newNode->setInput(_nodes[float4Index0], _nodes[float4Index1]);
	addNode(newNode);
	linkNode(float4Index0, nodeIndex);
	linkNode(float4Index1, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::addFloat(UINT floatIndex0, UINT floatIndex1)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeAddFloat>(newLocalVariableName);
	newNode->setInput(_nodes[floatIndex0], _nodes[floatIndex1]);
	addNode(newNode);
	linkNode(floatIndex0, nodeIndex);
	linkNode(floatIndex1, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::addFloat3(UINT float3Index0, UINT float3Index1)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeAddFloat3>(newLocalVariableName);
	newNode->setInput(_nodes[float3Index0], _nodes[float3Index1]);
	addNode(newNode);
	linkNode(float3Index0, nodeIndex);
	linkNode(float3Index1, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::multiplyFloat(UINT floatIndex0, UINT floatIndex1)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMultiplyFloat>(newLocalVariableName);
	newNode->setInput(_nodes[floatIndex0], _nodes[floatIndex1]);
	addNode(newNode);
	linkNode(floatIndex0, nodeIndex);
	linkNode(floatIndex1, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::multiplyFloat3ByScalar(UINT float3Index, UINT floatIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMultiplyFloat3ByScalar>(newLocalVariableName);
	newNode->setInputFloat3(_nodes[float3Index]);
	newNode->setInputScalar(_nodes[floatIndex]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
	linkNode(floatIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::maskX(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMaskX>(newLocalVariableName);
	newNode->setInput(_nodes[float4Index]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::maskY(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMaskY>(newLocalVariableName);
	newNode->setInput(_nodes[float4Index]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::maskZ(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMaskZ>(newLocalVariableName);
	newNode->setInput(_nodes[float4Index]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::maskW(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMaskW>(newLocalVariableName);
	newNode->setInput(_nodes[float4Index]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::getDeltaTime()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloatByVariableName>(newLocalVariableName, "DeltaTime");
	addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::setColorOfFloat4(UINT float4Index, UINT colorIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetColorOfFloat4>(newLocalVariableName);
	newNode->setInputFloat4(_nodes[float4Index]);
	newNode->setInputColor(_nodes[colorIndex]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);
	linkNode(colorIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::makeFloat4ByColorAlpha(UINT float3Index, UINT floatIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMakeFloat4ByColorAlpha>(newLocalVariableName);
	newNode->setInput(_nodes[float3Index], _nodes[floatIndex]);
	addNode(newNode);
	linkNode(float3Index, nodeIndex);
	linkNode(floatIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::makeFloat3(UINT floatXIndex, UINT floatYIndex, UINT floatZIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMakeFloat3>(newLocalVariableName);
	newNode->setInput(_nodes[floatXIndex], _nodes[floatYIndex], _nodes[floatZIndex]);
	addNode(newNode);
	linkNode(floatXIndex, nodeIndex);
	linkNode(floatYIndex, nodeIndex);
	linkNode(floatZIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::makeFloat4(UINT floatXIndex, UINT floatYIndex, UINT floatZIndex, UINT floatWIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeMakeFloat4>(newLocalVariableName);
	newNode->setInput(_nodes[floatXIndex], _nodes[floatYIndex], _nodes[floatZIndex], _nodes[floatWIndex]);
	addNode(newNode);
	linkNode(floatXIndex, nodeIndex);
	linkNode(floatYIndex, nodeIndex);
	linkNode(floatZIndex, nodeIndex);
	linkNode(floatWIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::sinByTime()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSinByTime>(newLocalVariableName);
	addNode(newNode);

	return nodeIndex;
}

void HlslGenerator::findNumRegisters()
{
	std::ifstream fin;
	fin.open(_baseShaderPath);
	assert(fin.is_open());

	constexpr UINT BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];

	const std::string registerPostfix = ")";
	const std::string cbvRegisterPrefix = "register(b";
	const std::string srvRegisterPrefix = "register(t";
	const std::string uavRegisterPrefix = "register(u";

	while (fin.getline(buffer, BUFFER_SIZE))
	{
		std::string line(buffer);

		_numSrvInBaseShader = max(_numSrvInBaseShader, parseNumBetween(line, srvRegisterPrefix, registerPostfix) + 1);
		_numUavInBaseShader = max(_numUavInBaseShader, parseNumBetween(line, uavRegisterPrefix, registerPostfix) + 1);
	}

	fin.close();
}

int HlslGenerator::parseNumBetween(std::string str, std::string prefix, std::string postfix)
{
	auto start = str.find(prefix);
	if (start != std::string::npos)
	{
		start += prefix.size();
		auto end = str.find(postfix, start);
		int length = end - start;

		// if "register(t10);", then registerNumber == "10"
		std::string numberStr = str.substr(start, length);
		int number = std::stoi(numberStr);
		
		return number;
	}

	return -1;
}

void HlslGenerator::insertStatements(std::ofstream& fout)
{
	const UINT numNodes = _graph.size();
	for (int i = 0; i < numNodes; ++i)
	{
		UINT nodeIndex = _topologicalOrder[i];
		fout << _nodes[nodeIndex]->generateStatements() << std::endl;
	}
}

void HlslGenerator::topologySort(UINT index)
{
	_visited[index] = true;
	for (auto linkIndex : _graph[index])
	{
		if (_visited[linkIndex])
			continue;
		topologySort(linkIndex);
	}

	_topologicalOrder.push_front(index);
}

//std::vector<ResourceRequest> HlslGenerator::collectSrvRequests()
//{
//	std::vector<ResourceRequest> result;
//
//	for (auto node : _nodes)
//	{
//		std::vector<ResourceRequest> requests = node->getResourceRequests();
//
//		for (auto request : requests)
//		{
//			if (request.type == ResourceViewType::Srv)
//			{
//				result.push_back(request);
//			}
//		}
//	}
//
//	return result;
//}

void HlslGenerator::insertSrvs(std::ofstream& fout)
{
	int numSrv = 0;

	for (auto node : _nodes)
	{
		int numResourcesToBind = node->getNumResourcesToBind();
		if (numResourcesToBind == 0)
			continue;

		std::vector<std::string> variableNames;

		for (int i = 0; i < numResourcesToBind; ++i)
		{
			if (node->getResourceViewType(i) == ResourceViewType::Srv) 
			{
				std::string type = node->getTypeInShader(i);
				std::string variableNameInShader = "texture" + std::to_string(numSrv);

				fout << type << " " << variableNameInShader << " : register(t" << _numSrvInBaseShader + numSrv << ");" << std::endl;
				++numSrv;

				variableNames.push_back(variableNameInShader);
			}
		}
		node->onResourceBound(variableNames);
	}
}

std::string HlslGenerator::getTypeInShader(ID3D12Resource* resource) const
{
	switch (resource->GetDesc().Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	{
		return "Texture2D";
	}

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
	{
		return "Texture3D";
	}

	default:
	{
		assert(0 && "Cannot get type in shader from resource.");
		return "ERROR";
	}
	}
}

std::string HlslGenerator::getNewLocalVariableName()
{
	return "local" + std::to_string(_nodes.size());
}

void HlslGenerator::addNode(std::shared_ptr<ShaderStatementNode> node)
{
	_nodes.push_back(node);
	_graph.emplace_back();
}

void HlslGenerator::linkNode(UINT from, UINT to)
{
	_graph[from].push_back(to);
}
