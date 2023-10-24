#include "Ui\NodeEditorEmit.h"

#include "Core/ParticleEmitter.h"
#include "Core/HlslTranslatorEmit.h"
#include "Ui/UiNodeFactory.h"
#include "Ui/NodeType.h"

#include "imgui.h"
#include "imnodes.h"

NodeEditorEmit::NodeEditorEmit(ParticleEmitter* emitter) :
    NodeEditor(),
    _emitter(emitter)
{
    const int node_id = _currentId;
    const auto createdNode =
        UiNodeFactory::createNode(node_id, NodeType::EmitterOutput);
    _nodes.push_back(createdNode);

    nextCurrentId(createdNode);
}

NodeEditorEmit::~NodeEditorEmit() = default;

void NodeEditorEmit::onCompileButtonClicked()
{
    HlslTranslatorEmit translator(_nodes, _links);

    _emitter->setShader(translator.compileShader());
}
