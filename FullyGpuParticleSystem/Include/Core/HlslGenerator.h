#pragma once

#include "Model/ResourceRequest.h"
#include "d3dcommon.h"

#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <wrl.h>

class ShaderStatementNode;

class HlslGenerator
{
public:
	HlslGenerator(std::wstring baseShaderPath);
	virtual ~HlslGenerator();

	void generateShaderFile(const std::wstring& outputPath);
	std::vector<std::shared_ptr<ShaderStatementNode>> getNodes();

	UINT empty();
	UINT newFloat(float x);
	UINT newFloat3(float x, float y, float z);
	UINT newFloat4(float x, float y, float z, float w);
	UINT randFloat();
	UINT randFloat3();
	UINT randFloat4();
	UINT setAlpha(UINT float4Index, UINT alphaIndex);
	UINT addFloat4(UINT float4Index0, UINT float4Index1);
	UINT addFloat3(UINT float3Index0, UINT float3Index1);
	UINT multiplyFloat(UINT floatIndex0, UINT floatIndex1);
	UINT multiplyFloat3ByScalar(UINT float3Index, UINT floatIndex);
	UINT branch(UINT aIndex, UINT bIndex, UINT aLargerThanB, UINT aEqualB, UINT bLargerThanA);
	UINT maskX(UINT float4Index);
	UINT maskY(UINT float4Index);
	UINT maskZ(UINT float4Index);
	UINT maskW(UINT float4Index);
	UINT getDeltaTime();
	UINT setColorOfFloat4(UINT float4Index, UINT colorIndex);
	UINT makeFloat4ByColorAlpha(UINT float3Index, UINT floatIndex);
	UINT makeFloat3(UINT floatXIndex, UINT floatYIndex, UINT floatZIndex);
	UINT makeFloat4(UINT floatXIndex, UINT floatYIndex, UINT floatZIndex, UINT floatWIndex);

protected:
	std::string getNewLocalVariableName();

	void addNode(std::shared_ptr<ShaderStatementNode> node);
	void linkNode(UINT from, UINT to);

protected:
	std::vector<std::shared_ptr<ShaderStatementNode>> _nodes;
	std::vector<std::vector<UINT>> _graph;

private:
	void findNumRegisters();
	int parseNumBetween(std::string str, std::string prefix, std::string postfix);

	void insertStatements(std::ofstream& fout);
	void topologySort(UINT index);

	std::vector<ResourceRequest> collectSrvRequests();

	void insertSrvs(std::ofstream& fout);
	std::string getTypeInShader(ID3D12Resource* resource) const; 

private:
	std::wstring _baseShaderPath;

	// for topological sort
	std::vector<bool> _visited;
	std::deque<UINT> _topologicalOrder;

	int _numSrvInBaseShader;
	int _numUavInBaseShader;

	std::vector<ResourceRequest> _requestedSrvResources;
};