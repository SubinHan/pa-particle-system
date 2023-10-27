#include "Ui/NodeEditorEmit.h"

#include "Core/ParticleEmitter.h"
#include "Core/HlslTranslatorEmit.h"
#include "Ui/UiNodeFactory.h"
#include "Ui/NodeType.h"

NodeEditorEmit::NodeEditorEmit(ParticleEmitter* emitter) :
    NodeEditor(),
    _emitter(emitter)
{
}

NodeEditorEmit::~NodeEditorEmit() = default;

std::string NodeEditorEmit::getName() const
{
    return _emitter->getName();
}

void NodeEditorEmit::onCompileButtonClicked()
{
    HlslTranslatorEmit translator(_nodes, _links);

    _emitter->setShader(translator.compileShader());
}

std::pair<std::vector<std::string>, std::vector<NodeType>> NodeEditorEmit::getCreatableNodes() const
{
    static const std::vector<std::string> creatableNodeNames =
    {
        "NewFloat",
        "NewFloat3",
        "NewFloat4",
        "RandFloat3",
        "AddFloat3",
    };

    static const std::vector<NodeType> creatableNodeTypes=
    {
        NodeType::NewFloat,
        NodeType::NewFloat3,
        NodeType::NewFloat4,
        NodeType::RandFloat3,
        NodeType::AddFloat3,
    };

    return std::make_pair<>(creatableNodeNames, creatableNodeTypes);
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
