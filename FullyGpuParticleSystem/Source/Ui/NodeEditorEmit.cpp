#include "Ui/NodeEditorEmit.h"

#include "Core/ParticleEmitter.h"
#include "Core/HlslTranslatorEmit.h"
#include "Ui/UiNodeFactory.h"
#include "Ui/NodeType.h"
#include "Ui/NodeEditorIo.h"

NodeEditorEmit::NodeEditorEmit(ParticleEmitter* emitter) :
    NodeEditor(),
    _emitter(emitter)
{
}

NodeEditorEmit::~NodeEditorEmit() = default;

std::string NodeEditorEmit::getName()
{
    return _emitter->getName();
}

void NodeEditorEmit::onCompileButtonClicked()
{
    HlslTranslatorEmit translator(_nodes, _links);

    _emitter->setShaderPs(translator.compileShader());
}

void NodeEditorEmit::load()
{
    NodeEditor::load();

    if (_nodes.empty())
    {
        createOutputNode();
    }
}

void NodeEditorEmit::createOutputNode()
{
    const int node_id = _currentId;
    const auto createdNode =
        UiNodeFactory::createNode(node_id, NodeType::EmitterOutput);
    _nodes.push_back(createdNode);

    nextCurrentId(createdNode);
}
