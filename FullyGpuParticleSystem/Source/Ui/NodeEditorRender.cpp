#include "Ui/NodeEditorRender.h"

#include "Core/ParticleRenderer.h"
#include "Core/HlslTranslatorRender.h"
#include "Ui/UiNodeFactory.h"
#include "Ui/NodeType.h"

NodeEditorRender::NodeEditorRender(ParticleRenderer* renderer) :
	NodeEditor(),
	_renderer(renderer)
{
}

NodeEditorRender::~NodeEditorRender() = default;

std::string NodeEditorRender::getName() const
{
	return _renderer->getName();
}

void NodeEditorRender::onCompileButtonClicked()
{
	HlslTranslatorRender translator(_nodes, _links);

	// TODO: change to translateTo
	_renderer->clearRegisteredShaderStatementNodes();
	auto blob = translator.compileShader();
	translator.registerTranslatedShaderNodesInto(_renderer);
	_renderer->setShaderPs(blob);
}

std::pair<std::vector<std::string>, std::vector<NodeType>> NodeEditorRender::getCreatableNodes() const
{
    static const std::vector<std::string> creatableNodeNames =
    {
		"GetParticleColor",
		"GetParticleAlpha",
		"SampleTexture2D",
		"SampleTexture2DSubUvAnmation",
		"GrayscaleToTranslucent",
    };

    static const std::vector<NodeType> creatableNodeTypes =
    {
		NodeType::GetParticleColor,
		NodeType::GetParticleAlpha,
        NodeType::SampleTexture2d,
		NodeType::SampleTexture2dSubUvAnimation,
        NodeType::GrayscaleToTranslucent,
    };

    return std::make_pair<>(creatableNodeNames, creatableNodeTypes);
}

void NodeEditorRender::load()
{
	NodeEditor::load();

	if (_nodes.empty())
	{
		createOutputNode();
	}
}

void NodeEditorRender::createOutputNode()
{
	const int node_id = _currentId;
	const auto createdNode =
		UiNodeFactory::createNode(node_id, NodeType::RendererOutput);
	_nodes.push_back(createdNode);

	nextCurrentId(createdNode);
}
