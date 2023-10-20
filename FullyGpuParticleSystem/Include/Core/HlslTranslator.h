#pragma once

#include "d3dcommon.h"

#include <string>
#include <vector>
#include <memory>
#include <wrl.h>

class ShaderStatementNode;

class HlslTranslator
{
public:
	HlslTranslator(std::wstring baseShaderPath);
	virtual ~HlslTranslator();

	Microsoft::WRL::ComPtr<ID3DBlob> compile(std::wstring outputPath);

	void generateShaderFile(std::wstring& outputPath);

	UINT newFloat1(float x);
	UINT newFloat3(float x, float y, float z);
	UINT newFloat4(float x, float y, float z, float w);
	UINT randFloat3();
	UINT randFloat4();
	UINT setAlpha(UINT float4Index, float value);
	UINT addFloat4(UINT float4Index0, UINT float4Index1);

protected:
	void addNode(std::shared_ptr<ShaderStatementNode> node);
	void linkNode(UINT from, UINT to);

protected:
	std::vector<std::shared_ptr<ShaderStatementNode>> _nodes;
	std::vector<std::vector<UINT>> _graph;

private:
	void insertCode(std::ofstream& fout);

	std::string getNewLocalVariableName();

private:
	std::wstring _baseShaderPath;
};