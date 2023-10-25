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
	virtual std::string getName() override;
	virtual void onCompileButtonClicked() override;

private:
	void createOutputNode();

private:
	ParticleEmitter* _emitter;
};