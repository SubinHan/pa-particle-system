#include "Core/HlslGenerator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetAlpha.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMultiplyFloat3ByScalar.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMaskX.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeMaskW.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeGetFloatByVariableName.h"
#include "Util/DxUtil.h"

#include <fstream>
#include <assert.h>
#include <queue>

HlslGenerator::HlslGenerator(std::wstring baseShaderPath)
	: _baseShaderPath(baseShaderPath)
{
}

HlslGenerator::~HlslGenerator() = default;

Microsoft::WRL::ComPtr<ID3DBlob> HlslGenerator::compile(std::wstring outputPath)
{
	generateShaderFile(outputPath);

	return nullptr;
}

void HlslGenerator::generateShaderFile(std::wstring& outputPath)
{
	std::ifstream fin;
	fin.open(_baseShaderPath);
	assert(fin.is_open());

	std::ofstream fout;
	fout.open(outputPath);
	assert(fout.is_open());

	constexpr UINT BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];

	while (fin.getline(buffer, BUFFER_SIZE))
	{
		std::string line(buffer);

		if (line.find("%s") == std::string::npos)
		{
			fout << buffer << std::endl;
			continue;
		}

		insertCode(fout);
	}

	fin.close();
	fout.close();
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

void HlslGenerator::insertCode(std::ofstream& fout)
{
	const UINT numNodes = _graph.size();
	_visited.resize(numNodes);
	std::fill(_visited.begin(), _visited.end(), false);

	for (int i = 0; i < numNodes; ++i)
	{
		if (_visited[i])
			continue;

		topologySort(i);
	}

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
