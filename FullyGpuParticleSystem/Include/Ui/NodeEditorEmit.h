#pragma once

#include "Ui/NodeEditor.h"

class ParticleEmitter;

class NodeEditorEmit : public NodeEditor
{
public:
	NodeEditorEmit(ParticleEmitter* emitter);
	virtual ~NodeEditorEmit();

protected:
	virtual void onCompileButtonClicked() override;

private:
	ParticleEmitter* _emitter;
};