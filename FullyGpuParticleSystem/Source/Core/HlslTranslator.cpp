#include "Core/HlslTranslator.h"

#include "Core/ShaderStatementNode/ShaderStatementNode.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat3.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeNewRandFloat4.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetAlpha.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeAddFloat4.h"
#include "Util/DxUtil.h"

#include <fstream>
#include <assert.h>
#include <queue>

HlslTranslator::HlslTranslator(std::wstring baseShaderPath)
	: _baseShaderPath(baseShaderPath)
{
}

HlslTranslator::~HlslTranslator() = default;

Microsoft::WRL::ComPtr<ID3DBlob> HlslTranslator::compile(std::wstring outputPath)
{
	generateShaderFile(outputPath);

	return nullptr;
}

void HlslTranslator::generateShaderFile(std::wstring& outputPath)
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

UINT HlslTranslator::newFloat1(float x)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewFloat>(newLocalVariableName, x);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslTranslator::newFloat3(float x, float y, float z)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewFloat3>(newLocalVariableName, x, y, z);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslTranslator::newFloat4(float x, float y, float z, float w)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode = 
		std::make_shared<ShaderStatementNodeNewFloat4>(newLocalVariableName, x, y, z, w);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslTranslator::randFloat3()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewRandFloat3>(newLocalVariableName);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslTranslator::randFloat4()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewRandFloat4>(newLocalVariableName);
	addNode(newNode);

	return nodeIndex;
}

UINT HlslTranslator::setAlpha(UINT float4Index, float value)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	const UINT nodeIndex = _nodes.size();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetAlpha>(newLocalVariableName, value);
	newNode->setInput(_nodes[float4Index]);
	addNode(newNode);
	linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslTranslator::addFloat4(UINT float4Index0, UINT float4Index1)
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

void HlslTranslator::insertCode(std::ofstream& fout)
{
	const UINT numNodes = _graph.size();

	// find root nodes
	std::vector<bool> isRoot(numNodes, true);

	for (int i = 0; i < numNodes; ++i)
	{
		for (int j = 0; j < _graph[i].size(); ++j)
		{
			isRoot[_graph[i][j]] = false;
		}
	}

	std::queue<UINT> q;

	for (int i = 0; i < numNodes; ++i)
	{
		if (isRoot[i])
			q.push(i);
	}

	std::vector<bool> visited(numNodes);

	// bfs
	while (!q.empty())
	{
		const UINT nodeIndex = q.front();
		q.pop();

		for (auto linkIndex : _graph[nodeIndex])
		{
			if (visited[linkIndex])
				continue;

			visited[linkIndex] = true;
			q.push(linkIndex);
		}

		fout << _nodes[nodeIndex]->generateStatements() << std::endl;
	}
}

std::string HlslTranslator::getNewLocalVariableName()
{
	return "local" + std::to_string(_nodes.size());
}

void HlslTranslator::addNode(std::shared_ptr<ShaderStatementNode> node)
{
	_nodes.push_back(node);
	_graph.emplace_back();
}

void HlslTranslator::linkNode(UINT from, UINT to)
{
	_graph[from].push_back(to);
}
