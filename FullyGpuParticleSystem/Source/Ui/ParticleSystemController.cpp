#include "Ui/ParticleSystemController.h"

#include "Core/ParticleSystem.h"
#include "Ui/NodeEditorEmit.h"
#include "Ui/NodeEditorSimulate.h"
#include "Ui/NodeEditorRender.h"

#include "imgui.h"

ParticleSystemController::ParticleSystemController(std::vector<std::unique_ptr<ParticleSystem>>* particleSystems) :
	_particleSystems(particleSystems)
{
}

ParticleSystemController::~ParticleSystemController() = default;

void ParticleSystemController::show()
{
	ImGui::Begin("Particle Controller");
	if (ImGui::Button("New Particle System"))
	{
		// add a new particle system.
		std::string particleSystemName = "ParticleSystem" + std::to_string(_particleSystems->size());

	}

	for (int i = 0; i < _particleSystems->size(); ++i)
	{
		std::string particleSystemName = (*_particleSystems)[i]->getName();
		if (ImGui::TreeNode(particleSystemName.c_str()))
		{
			if (ImGui::Button("Particle Emitter"))
			{
				// open particle emitter editor

				auto editor = 
					std::make_unique<NodeEditorEmit>((*_particleSystems)[i]->getEmitter());
				editor->load();
				_showingWindow.push_back(std::move(editor));
			}

			if (ImGui::Button("Particle Simulator"))
			{
				// open particle simulator editor

				auto editor =
					std::make_unique<NodeEditorSimulate>((*_particleSystems)[i]->getSimulator());
				editor->load();
				_showingWindow.push_back(std::move(editor));
			}

			if (ImGui::Button("Particle Renderer"))
			{
				// open particle renderer editor

				auto editor =
					std::make_unique<NodeEditorRender>((*_particleSystems)[i]->getRenderer());
				editor->load();
				_showingWindow.push_back(std::move(editor));
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();

	for (int i = 0; i < _showingWindow.size(); ++i)
	{
		if (!_showingWindow[i]->isAlive())
		{
			_showingWindow.erase(_showingWindow.begin() + i);

			continue;
		}

		_showingWindow[i]->show();
	}
}

bool ParticleSystemController::isAlive()
{
	return true;
}
