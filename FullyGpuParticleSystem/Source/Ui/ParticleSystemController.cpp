#include "Ui/ParticleSystemController.h"

#include "Core/ParticleSystem.h"
#include "Core/ParticleSystemManager.h"
#include "Core/ParticleRenderer.h"
#include "Ui/NodeEditorEmit.h"
#include "Ui/NodeEditorSimulate.h"
#include "Ui/NodeEditorRender.h"
#include "Io/ParticleSystemIo.h"

#include "imgui.h"

ParticleSystemController::ParticleSystemController(ParticleSystemManager* particleSystemManager) :
	_particleSystemManager(particleSystemManager)
{
	loadParticleSystemManager();
}

ParticleSystemController::~ParticleSystemController() = default;

void ParticleSystemController::show()
{
	ImGui::Begin("Particle Controller");
	if (ImGui::Button("Save"))
	{
		saveParticleSystemManager();
	}
	if (ImGui::Button("New Particle System"))
	{
		// add a new particle system.
		int index = _particleSystemManager->getNumParticleSystems();
		std::string particleSystemName = "ParticleSystem" + std::to_string(index);

		createNewParticleSystem(particleSystemName);
	}

	for (int i = 0; i < _particleSystemManager->getNumParticleSystems(); ++i)
	{
		auto particleSystem = _particleSystemManager->getParticleSystemByIndex(i);

		std::string particleSystemName = particleSystem->getName();
		bool isAlive = true;
		if (ImGui::CollapsingHeader(particleSystemName.c_str(), &isAlive))
		{
			if (ImGui::TreeNode("Particle Emitter"))
			{
				_spawnRate[i] = particleSystem->getSpawnRate();
				ImGui::DragFloat("Spawn rate", &_spawnRate[i], 1.0f, 0.0f, 10000.0f);
				particleSystem->setSpawnRate(_spawnRate[i]);

				if (ImGui::Button("Modify Shader"))
				{
					// open particle emitter editor

					auto editor =
						std::make_unique<NodeEditorEmit>(particleSystem->getEmitter());
					editor->load();
					_showingWindow = std::move(editor);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Particle Simulator"))
			{
				if (ImGui::Button("Modify Shader"))
				{
					// open particle simulator editor

					auto editor =
						std::make_unique<NodeEditorSimulate>(particleSystem->getSimulator());
					editor->load();
					_showingWindow = std::move(editor);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Particle Renderer"))
			{
				bool isOpaque = particleSystem->getRenderer()->isOpaque();
				ImGui::Checkbox("isOpaque", &isOpaque);
				particleSystem->getRenderer()->setOpaque(isOpaque);

				const char* items[] = { "Sprite", "Quad", "Mesh", "RibbonTrail" };
				static int item_current_idx = 0; // Here we store our selection data as an index.
				const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)

				static ImGuiComboFlags flags = 0;
				//flags &= ~(ImGuiComboFlags_HeightMask_ & ~ImGuiComboFlags_HeightRegular);

				if (ImGui::BeginCombo("renderer", combo_preview_value, flags))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						const bool is_selected = (item_current_idx == n);
						if (ImGui::Selectable(items[n], is_selected))
							item_current_idx = n;

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Modify Shader"))
				{
					// open particle renderer editor

					auto editor =
						std::make_unique<NodeEditorRender>(particleSystem->getRenderer());
					editor->load();
					_showingWindow = std::move(editor);
				}
				ImGui::TreePop();
			}
		}

		// TODO: not works. should fix logic.
		// store isAlive per particle system and save the state about popup is opened or not.
		if (!isAlive)
		{
			ImGui::OpenPopup("confirmDeleteParticleSystem");
			if (ImGui::BeginPopup("confirmDeleteParticleSystem"))
			{
				ImGui::Text("Are you sure to delete the particle system?");
				if (ImGui::Button("Yes"))
				{
					// delete particle system.
					deleteParticleSystemAndSaveFiles(i);
					i--;
					ImGui::EndPopup();
				}
				else if (ImGui::Button("No"))
				{
					ImGui::EndPopup();
				}
				else
				{
					ImGui::EndPopup();
				}
			}

		}
	}
	ImGui::End();

	if (_showingWindow)
	{
		if (!_showingWindow->isAlive())
		{
			_showingWindow = nullptr;
		}
		else
		{
			_showingWindow->show();
		}
	}
}

bool ParticleSystemController::isAlive()
{
	return true;
}

void ParticleSystemController::saveParticleSystemManager()
{
	for (int i = 0; i < _particleSystemManager->getNumParticleSystems(); ++i)
	{
		ParticleSystemIo::save("Saved/", _particleSystemManager->getParticleSystemByIndex(i));
	}
}

void ParticleSystemController::loadParticleSystemManager()
{
	auto info = ParticleSystemIo::getSavedParticleSystems("Saved/");

	for (int i = 0; i < info.size(); ++i)
	{
		auto [filePath, particleSystemName] = info[i];

		auto particleSystem = createNewParticleSystem(particleSystemName);
		ParticleSystemIo::loadInto(filePath, particleSystem);
	}
}

ParticleSystem* ParticleSystemController::createNewParticleSystem(std::string name)
{
	int index = _particleSystemManager->getNumParticleSystems();
	_particleSystemManager->createNewParticleSystem(name);
	auto particleSystem = _particleSystemManager->getParticleSystemByIndex(index);
	_spawnRate.push_back(particleSystem->getSpawnRate());

	return particleSystem;
}

void ParticleSystemController::deleteParticleSystemAndSaveFiles(int index)
{


	// TODO
}
