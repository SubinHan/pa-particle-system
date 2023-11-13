#pragma once

#include "Ui/NodeEditor.h"

class ParticleRenderPass;

class NodeEditorRender : public NodeEditor
{
public:
	NodeEditorRender(ParticleRenderPass* renderer);
	virtual ~NodeEditorRender();

	virtual void load() override;

protected:
	virtual std::string getName() const override;
	virtual void onCompileButtonClicked() override;
	virtual std::pair<std::vector<std::string>, std::vector<NodeType>> getCreatableNodes() const override;

private:
	void createOutputNode();

private:
	ParticleRenderPass* _renderer;
};