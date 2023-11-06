#pragma once

#include "Core/ParticlePass.h"
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
	void translateTo(ParticlePass* pass);
	void registerTranslatedShaderNodesInto(ParticlePass* pass);

protected:
	virtual std::unique_ptr<HlslGenerator> createHlslGenerator() = 0;
	virtual Microsoft::WRL::ComPtr<ID3DBlob> compileShaderImpl(std::wstring shaderPath) = 0;

	//virtual std::pair<UiNode, int> getFinalOutputNode() = 0;

	virtual bool translateNode(UiNode node);

	std::vector<int> findLinkedNodesWithOutputOf(const int nodeIndex);
	std::vector<int> findLinkedNodesWithInputOf(const int nodeIndex);
	int findNodeIdLinkedAsOutput(UiLink link);
	int findOppositeNodeIdByInputAttrbuteId(int inputId);

protected:
	std::vector<UiNode> _nodes;
	std::vector<UiLink> _links;

	std::unique_ptr<HlslGenerator> _hlslGenerator;

	// key: ui node index, value: hlsl index
	std::unordered_map<int, UINT> indexMap;


private:
	void generateNodes();
	void generateShaderFile(std::wstring shaderPath);

	void removeOrphanNodes();
	void removeOrphanNodes0(const int nodeIndex);
	void topologySort();
	void topologySort0(const int index);

	int getIndex(int nodeId);

private:

	std::vector<bool> _visited;
	std::vector<bool> _isOrphan;
	std::deque<int> _topologicalOrder;
};