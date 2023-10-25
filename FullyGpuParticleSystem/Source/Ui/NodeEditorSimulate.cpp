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

std::string NodeEditorSimulate::getName()
{
	return _simulator->getName();
}

void NodeEditorSimulate::onCompileButtonClicked()
{
	HlslTranslatorSimulate translator(_nodes, _links);

	_simulator->setShaderPs(translator.compileShader());
}

void NodeEditorSimulate::load()
{
	NodeEditor::load();

	if (_nodes.empty())
	{
		createOutputNode();
	}
}

void NodeEditorSimulate::createOutputNode()
{
	const int node_id = _currentId;
	const auto createdNode =
		UiNodeFactory::createNode(node_id, NodeType::SimulatorOutput);
	_nodes.push_back(createdNode);

	nextCurrentId(createdNode);
}
