#pragma once

#include "Ui/NodeEditor.h"

class ParticleRenderer;

class NodeEditorRender : public NodeEditor
{
public:
	NodeEditorRender(ParticleRenderer* renderer);
	virtual ~NodeEditorRender();

	virtual void load() override;

protected:
	virtual std::string getName() override;
	virtual void onCompileButtonClicked() override;

private:
	void createOutputNode();

private:
	ParticleRenderer* _renderer;
};