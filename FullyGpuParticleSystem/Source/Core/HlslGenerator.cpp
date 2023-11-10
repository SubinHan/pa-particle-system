#include "Core/HlslGenerator.h"

#include "Core/ShaderStatementGraph.h"
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
	_graph(std::make_shared<ShaderStatementGraph>()),
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

std::shared_ptr<ShaderStatementGraph> HlslGenerator::getShaderStatementGraph()
{
	return _graph;
}


UINT HlslGenerator::empty()
{
	auto newNode =
		std::make_shared<ShaderStatementNodeEmpty>();
	const UINT nodeIndex = _graph->addNode(newNode);
	return nodeIndex;
}

UINT HlslGenerator::newFloat(float x)
{
	std::string newLocalVariableName = getNewLocalVariableName();

	auto newNode =
		std::make_shared<ShaderStatementNodeNewFloat>(newLocalVariableName, x);
	const UINT nodeIndex = _graph->addNode(newNode);
	return nodeIndex;
}

UINT HlslGenerator::newFloat3(float x, float y, float z)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewFloat3>(newLocalVariableName, x, y, z);
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::newFloat4(float x, float y, float z, float w)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewFloat4>(newLocalVariableName, x, y, z, w);
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::randFloat()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewRandFloat>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::randFloat3()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewRandFloat3>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::randFloat4()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeNewRandFloat4>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);

	return nodeIndex;
}

UINT HlslGenerator::setAlpha(UINT float4Index, UINT alphaIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetAlpha>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInputFloat4(_graph->getNode(float4Index));
	newNode->setInputAlpha(_graph->getNode(alphaIndex));

	_graph->linkNode(float4Index, nodeIndex);
	_graph->linkNode(alphaIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::addFloat4(UINT float4Index0, UINT float4Index1)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeAddFloat4>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float4Index0), _graph->getNode(float4Index1));

	_graph->linkNode(float4Index0, nodeIndex);
	_graph->linkNode(float4Index1, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::addFloat(UINT floatIndex0, UINT floatIndex1)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeAddFloat>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(floatIndex0), _graph->getNode(floatIndex1));

	_graph->linkNode(floatIndex0, nodeIndex);
	_graph->linkNode(floatIndex1, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::addFloat3(UINT float3Index0, UINT float3Index1)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeAddFloat3>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float3Index0), _graph->getNode(float3Index1));

	_graph->linkNode(float3Index0, nodeIndex);
	_graph->linkNode(float3Index1, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::multiplyFloat(UINT floatIndex0, UINT floatIndex1)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMultiplyFloat>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(floatIndex0), _graph->getNode(floatIndex1));

	_graph->linkNode(floatIndex0, nodeIndex);
	_graph->linkNode(floatIndex1, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::multiplyFloat3ByScalar(UINT float3Index, UINT floatIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMultiplyFloat3ByScalar>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInputFloat3(_graph->getNode(float3Index));
	newNode->setInputScalar(_graph->getNode(floatIndex));

	_graph->linkNode(float3Index, nodeIndex);
	_graph->linkNode(floatIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::maskX(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMaskX>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float4Index));

	_graph->linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::maskY(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMaskY>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float4Index));

	_graph->linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::maskZ(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMaskZ>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float4Index));

	_graph->linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::maskW(UINT float4Index)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMaskW>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float4Index));

	_graph->linkNode(float4Index, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::getDeltaTime()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeGetFloatByVariableName>(newLocalVariableName, "DeltaTime");
	const UINT nodeIndex =	_graph->addNode(newNode);


	return nodeIndex;
}

UINT HlslGenerator::setColorOfFloat4(UINT float4Index, UINT colorIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeSetColorOfFloat4>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInputFloat4(_graph->getNode(float4Index));
	newNode->setInputColor(_graph->getNode(colorIndex));

	_graph->linkNode(float4Index, nodeIndex);
	_graph->linkNode(colorIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::makeFloat4ByColorAlpha(UINT float3Index, UINT floatIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMakeFloat4ByColorAlpha>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(float3Index), _graph->getNode(floatIndex));

	_graph->linkNode(float3Index, nodeIndex);
	_graph->linkNode(floatIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::makeFloat3(UINT floatXIndex, UINT floatYIndex, UINT floatZIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMakeFloat3>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(floatXIndex), _graph->getNode(floatYIndex), _graph->getNode(floatZIndex));

	_graph->linkNode(floatXIndex, nodeIndex);
	_graph->linkNode(floatYIndex, nodeIndex);
	_graph->linkNode(floatZIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::makeFloat4(UINT floatXIndex, UINT floatYIndex, UINT floatZIndex, UINT floatWIndex)
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeMakeFloat4>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);
	newNode->setInput(_graph->getNode(floatXIndex), _graph->getNode(floatYIndex), _graph->getNode(floatZIndex), _graph->getNode(floatWIndex));

	_graph->linkNode(floatXIndex, nodeIndex);
	_graph->linkNode(floatYIndex, nodeIndex);
	_graph->linkNode(floatZIndex, nodeIndex);
	_graph->linkNode(floatWIndex, nodeIndex);

	return nodeIndex;
}

UINT HlslGenerator::sinByTime()
{
	std::string newLocalVariableName = getNewLocalVariableName();
	auto newNode =
		std::make_shared<ShaderStatementNodeSinByTime>(newLocalVariableName);
	const UINT nodeIndex = _graph->addNode(newNode);


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
	const UINT numNodes = _graph->getSize();
	const auto topologicalOrder = _graph->topologicalOrder();

	for (int i = 0; i < numNodes; ++i)
	{
		UINT nodeIndex = topologicalOrder[i];
		fout << _graph->getNode(nodeIndex)->generateStatements() << std::endl;
	}
}

void HlslGenerator::insertSrvs(std::ofstream& fout)
{
	int numSrv = 0;

	for (int i = 0; i < _graph->getSize(); ++i)
	{
		auto node = _graph->getNode(i);

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
	return "local" + std::to_string(_graph->getSize());
}
