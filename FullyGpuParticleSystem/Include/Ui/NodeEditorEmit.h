#pragma once

#include "Ui/NodeEditor.h"

class ParticleEmitter;

class NodeEditorEmit : public NodeEditor
{
public:
	NodeEditorEmit(ParticleEmitter* emitter);
	virtual ~NodeEditorEmit();

	virtual void load() override;

protected:
	virtual std::string getName() const override;
	virtual void onCompileButtonClicked() override;
	virtual std::pair<std::vector<std::string>, std::vector<NodeType>> getCreatableNodes() const override;

private:
	void createOutputNode();

private:
	ParticleEmitter* _emitter;
};