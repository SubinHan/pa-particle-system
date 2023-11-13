#include "Core/ShaderStatementGraph.h"

ShaderStatementGraph::ShaderStatementGraph() :
	_nodes(),
	_graph(),
	_visited(),
	_topologicalOrder(),
	_isDirty(false)
{

}

ShaderStatementGraph::~ShaderStatementGraph() = default;

UINT ShaderStatementGraph::addNode(std::shared_ptr<ShaderStatementNode> node)
{
	_nodes.push_back(node);
	_graph.emplace_back();

	_isDirty = true;

	return _nodes.size() - 1;
}

void ShaderStatementGraph::linkNode(UINT from, UINT to)
{
	_graph[from].push_back(to);
}

UINT ShaderStatementGraph::getSize()
{
	return _nodes.size();
}

std::shared_ptr<ShaderStatementNode> ShaderStatementGraph::getNode(UINT index)
{
	return _nodes[index];
}

std::deque<UINT> ShaderStatementGraph::topologicalOrder()
{
	if (!_isDirty)
		return _topologicalOrder;

	topologySort();

	return _topologicalOrder;
}

void ShaderStatementGraph::topologySort()
{
	_topologicalOrder.clear();
	_visited.clear();
	_visited.resize(getSize());
	for (int i = 0; i < getSize(); ++i)
	{
		if (_visited[i])
			continue;

		topologySort0(i);
	}
}

void ShaderStatementGraph::topologySort0(UINT index)
{
	_visited[index] = true;
	for (auto linkIndex : _graph[index])
	{
		if (_visited[linkIndex])
			continue;
		topologySort0(linkIndex);
	}

	_topologicalOrder.push_front(index);
}
