#pragma once

#include "Ui/NodeEditor.h"

class ParticleSimulator;

class NodeEditorSimulate : public NodeEditor
{
public:
	NodeEditorSimulate(ParticleSimulator* simulator);
	virtual ~NodeEditorSimulate();

	virtual void load() override;

protected:
	virtual std::string getName() override;
	virtual void onCompileButtonClicked() override;

private:
	void createOutputNode();

private:
	ParticleSimulator* _simulator;
};