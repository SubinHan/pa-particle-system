#include "Ui/NodeEditorSimulate.h"

#include "Core/ParticleSimulator.h"
#include "Core/HlslTranslatorSimulate.h"
#include "Ui/UiNodeFactory.h"
#include "Ui/NodeType.h"

NodeEditorSimulate::NodeEditorSimulate(ParticleSimulator* simulator) :
	NodeEditor(),
	_simulator(simulator)
{
}

NodeEditorSimulate::~NodeEditorSimulate() = default;

std::string NodeEditorSimulate::getName() const
{
	return _simulator->getName();
}

void NodeEditorSimulate::onCompileButtonClicked()
{
	HlslTranslatorSimulate translator(_nodes, _links);
	translator.translateTo(_simulator);
}

std::pair<std::vector<std::string>, std::vector<NodeType>> NodeEditorSimulate::getCreatableNodes() const
{
	static const std::vector<std::string> creatableNodeNames =
	{
		"PointAttractionForce",
		"DragForce",
		"VortexForce",
		"CurlNoiseForce",
	};

	static const std::vector<NodeType> creatableNodeTypes =
	{
		NodeType::PointAttractionForce,
		NodeType::DragForce,
		NodeType::VortexForce,
		NodeType::CurlNoiseForce,
	};

	return std::make_pair<>(creatableNodeNames, creatableNodeTypes);
}

void NodeEditorSimulate::load()
{
	NodeEditor::load();

	if (_nodes.empty())
	{
		// Output node should be 0 id, so create first.
		createOutputNode();
		createInputNode();
	}
}

void NodeEditorSimulate::createInputNode()
{
	const int node_id = _currentId;
	const auto createdNode =
		UiNodeFactory::createNode(node_id, NodeType::StartParticleSimulation);
	_nodes.push_back(createdNode);

	nextCurrentId(createdNode);
}

void NodeEditorSimulate::createOutputNode()
{
	const int node_id = _currentId;
	const auto createdNode =
		UiNodeFactory::createNode(node_id, NodeType::SimulatorOutput);
	_nodes.push_back(createdNode);

	nextCurrentId(createdNode);
}
