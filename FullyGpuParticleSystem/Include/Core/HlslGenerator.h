#pragma once

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

	Microsoft::WRL::ComPtr<ID3DBlob> compile(std::wstring outputPath);

	void generateShaderFile(std::wstring& outputPath);

	UINT newFloat(float x);
	UINT newFloat3(float x, float y, float z);
	UINT newFloat4(float x, float y, float z, float w);
	UINT randFloat3();
	UINT randFloat4();
	UINT setAlpha(UINT float4Index, UINT alphaIndex);
	UINT addFloat4(UINT float4Index0, UINT float4Index1);
	UINT addFloat3(UINT float3Index0, UINT float3Index1);
	UINT multiplyFloat3ByScalar(UINT float3Index, UINT floatIndex);
	UINT branch(UINT aIndex, UINT bIndex, UINT aLargerThanB, UINT aEqualB, UINT bLargerThanA);
	UINT maskX(UINT float4Index);
	UINT maskW(UINT float4Index);
	UINT getDeltaTime();

protected:
	std::string getNewLocalVariableName();

	void addNode(std::shared_ptr<ShaderStatementNode> node);
	void linkNode(UINT from, UINT to);

protected:
	std::vector<std::shared_ptr<ShaderStatementNode>> _nodes;
	std::vector<std::vector<UINT>> _graph;

private:
	void insertCode(std::ofstream& fout);
	void topologySort(UINT index);

private:
	std::wstring _baseShaderPath;

	// for topological sort
	std::vector<bool> _visited;
	std::deque<UINT> _topologicalOrder;
};