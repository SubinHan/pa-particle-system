#pragma once

#include "Model/ResourceRequest.h"
#include "d3dcommon.h"

#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <wrl.h>

class ShaderStatementNode;
class ShaderStatementGraph;

class HlslGenerator
{
public:
	HlslGenerator(std::wstring baseShaderPath);
	virtual ~HlslGenerator();

	void generateShaderFile(const std::wstring& outputPath);
	std::shared_ptr<ShaderStatementGraph> getShaderStatementGraph();

	UINT empty();
	UINT newFloat(float x);
	UINT newFloat3(float x, float y, float z);
	UINT newFloat4(float x, float y, float z, float w);
	UINT randFloat();
	UINT randFloat3();
	UINT randFloat4();
	UINT setAlpha(UINT float4Index, UINT alphaIndex);
	UINT addFloat(UINT floatIndex0, UINT floatIndex1);
	UINT addFloat3(UINT float3Index0, UINT float3Index1);
	UINT addFloat4(UINT float4Index0, UINT float4Index1);
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
	UINT sinByTime();

protected:
	std::string getNewLocalVariableName();

	std::shared_ptr<ShaderStatementGraph> _graph;

private:
	void findNumRegisters();
	int parseNumBetween(std::string str, std::string prefix, std::string postfix);

	void insertStatements(std::ofstream& fout);

	std::vector<ResourceRequest> collectSrvRequests();

	void insertSrvs(std::ofstream& fout);
	std::string getTypeInShader(ID3D12Resource* resource) const; 

private:
	std::wstring _baseShaderPath;


	int _numSrvInBaseShader;
	int _numUavInBaseShader;

	std::vector<ResourceRequest> _requestedSrvResources;
};