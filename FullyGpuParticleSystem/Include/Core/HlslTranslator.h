#pragma once

#include "Core/Hashable.h"
#include "Ui/UiNode.h"
#include "Ui/UiLink.h"
#include "Util/DxUtil.h"

#include <vector>
#include <deque>
#include <unordered_map>

class HlslGenerator;

class HlslTranslator : public Hashable
{
public:
	HlslTranslator(std::vector<UiNode> nodes, std::vector<UiLink> links);
	virtual ~HlslTranslator();

	Microsoft::WRL::ComPtr<ID3DBlob> compileShader();

protected:
	virtual std::unique_ptr<HlslGenerator> createHlslGenerator() = 0;
	virtual Microsoft::WRL::ComPtr<ID3DBlob> compileShaderImpl(std::wstring shaderPath) = 0;

	virtual bool generateNode(UiNode node);

	std::vector<int> findLinkedNodesWithOutput(const int nodeIndex);
	int findNodeIdLinkedAsOutput(UiLink link);
	int findOppositeNodeByInputAttrbuteId(int inputId);

protected:
	std::vector<UiNode> _nodes;
	std::vector<UiLink> _links;

	std::unique_ptr<HlslGenerator> _hlslGenerator;

	// key: ui node index, value: hlsl index
	std::unordered_map<int, UINT> indexMap;


private:
	void generateNodes();
	void generateShaderFile(std::wstring shaderPath);

	void topologySort();
	void topologySort0(const int index);

private:

	std::vector<bool> _visited;
	std::deque<int> _topologicalOrder;
};