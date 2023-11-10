#pragma once

#include "d3dcommon.h"

#include <memory>
#include <vector>
#include <deque>

class ShaderStatementNode;

class ShaderStatementGraph
{
public:
	ShaderStatementGraph();
	~ShaderStatementGraph();

	UINT addNode(std::shared_ptr<ShaderStatementNode> node);
	void linkNode(UINT from, UINT to);

	UINT getSize();
	std::shared_ptr<ShaderStatementNode> getNode(UINT index);

	std::deque<UINT> topologicalOrder();

private:
	void topologySort();
	void topologySort0(UINT index);

private:
	std::vector<std::shared_ptr<ShaderStatementNode>> _nodes;
	std::vector<std::vector<UINT>> _graph;

	std::vector<bool> _visited;
	std::deque<UINT> _topologicalOrder;

	bool _isDirty;
};