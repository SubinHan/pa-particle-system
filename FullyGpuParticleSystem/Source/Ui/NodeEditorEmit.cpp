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
    translator.translateTo(_emitter);
}

std::pair<std::vector<std::string>, std::vector<NodeType>> NodeEditorEmit::getCreatableNodes() const
{
    static const std::vector<std::string> creatableNodeNames =
    {
        "NewFloat",
        "NewFloat3",
        "NewFloat4",
        "RandFloat",
        "RandFloat3",
        "AddFloat3",
        "MaskX",
        "MaskY",
        "MaskZ",
        "MaskW",
        "MakeFloat3",
        "MakeFloat4",
        "MultiplyFloat3ByScalar",
        "MultiplyFloat",
    };

    static const std::vector<NodeType> creatableNodeTypes=
    {
        NodeType::NewFloat,
        NodeType::NewFloat3,
        NodeType::NewFloat4,
        NodeType::RandFloat,
        NodeType::RandFloat3,
        NodeType::AddFloat3,
        NodeType::MaskX,
        NodeType::MaskY,
        NodeType::MaskZ,
        NodeType::MaskW,
        NodeType::MakeFloat3,
        NodeType::MakeFloat4,
        NodeType::MultiplyFloat3ByScalar,
        NodeType::MultiplyFloat,
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
