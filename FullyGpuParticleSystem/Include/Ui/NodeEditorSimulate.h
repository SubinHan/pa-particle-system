#pragma once

#include "Ui/NodeEditor.h"

class ParticleDestroyer;
class ParticleAliveMover;

class NodeEditorSimulate : public NodeEditor
{
public:
	NodeEditorSimulate(ParticleDestroyer* destroyer, ParticleAliveMover* aliveMover);
	virtual ~NodeEditorSimulate();

	virtual void load() override;

protected:
	virtual std::string getName() const override;
	virtual void onCompileButtonClicked() override;
	virtual std::pair<std::vector<std::string>, std::vector<NodeType>> getCreatableNodes() const override;

private:
	void createInputNode();
	void createOutputNode();

private:
	ParticleDestroyer* _destroyer;
	ParticleAliveMover* _aliveMover;
};