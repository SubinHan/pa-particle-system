#include "Ui/ParticleSystemController.h"

#include "Core/ParticleSystem.h"
#include "Core/ParticleSystemManager.h"
#include "Core/ParticleRenderPass.h"
#include "Core/TextureManager.h"
#include "Core/ParticleDestroyer.h"
#include "Core/ParticleAliveMover.h"
#include "Model/RendererType.h"
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
				float spawnRate = particleSystem->getSpawnRate();
				ImGui::DragFloat("Spawn rate", &spawnRate, 1.0f, 0.0f, 10000.0f);
				particleSystem->setSpawnRate(spawnRate);

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

					auto editor = std::make_unique<NodeEditorSimulate>(
						particleSystem->getDestroyer(), 
						particleSystem->getDestroyer()->getParticleAliveMover());
					editor->load();
					_showingWindow = std::move(editor);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Particle Renderer"))
			{				
				showCheckboxToSelectOpaqueness(particleSystem);
				showCheckboxToSelectWireframe(particleSystem);
				showComboToSelectBoundingMode(particleSystem);
				showComboToSelectRenderer(particleSystem);

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

void ParticleSystemController::showComboToSelectBoundingMode(ParticleSystem* particleSystem)
{
	auto textureManager = TextureManager::getInstance();
	auto textureNames = textureManager->getTextureNames();

	std::vector<std::string> items = { "NoBounding" };
	items.insert(items.end(), textureNames.begin(), textureNames.end());

	int currentItemIndex = 0;

	if (!particleSystem->getRenderer()->isBounding())
	{
		currentItemIndex = 0;
	}
	else
	{
		for (int i = 1; i < items.size(); ++i)
		{
			if (particleSystem->getRenderer()->getBoundingTextureName() == items[i])
			{
				currentItemIndex = i;
			}
		}
	}

	static ImGuiComboFlags flags = 0;

	if (ImGui::BeginCombo("boundingMode", items[currentItemIndex].data(), flags))
	{
		for (int n = 0; n < items.size(); n++)
		{
			const bool isSelected = (currentItemIndex == n);
			if (ImGui::Selectable(items[n].data(), isSelected))
			{
				currentItemIndex = n;

				if (n == 0)
				{
					// no bounding
					particleSystem->getRenderer()->setBoundingMode(false);
				}
				else
				{
					particleSystem->getRenderer()->setBoundingMode(true);
					particleSystem->getRenderer()->setBoundingTextureName(items[n]);
				}
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void ParticleSystemController::showComboToSelectRenderer(ParticleSystem* particleSystem)
{
	const char* items[] = { "Sprite", "Ribbon" };
	int item_current_idx = static_cast<int>(particleSystem->getRendererType());
	const char* combo_preview_value = items[item_current_idx];

	static ImGuiComboFlags flags = 0;

	if (ImGui::BeginCombo("renderer", combo_preview_value, flags))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
			{
				item_current_idx = n;
				if (item_current_idx == 0)
				{
					particleSystem->setRendererType(RendererType::Sprite);
				}
				if (item_current_idx == 1)
				{
					particleSystem->setRendererType(RendererType::Ribbon);
				}
			}

			showConfigWidgetsOfRenderer(static_cast<RendererType>(item_current_idx));

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
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

	return particleSystem;
}

void ParticleSystemController::deleteParticleSystemAndSaveFiles(int index)
{


	// TODO
}

void ParticleSystemController::showCheckboxToSelectOpaqueness(ParticleSystem* particleSystem)
{
	bool isOpaque = particleSystem->getRenderer()->isOpaque();
	ImGui::Checkbox("isOpaque", &isOpaque);
	particleSystem->getRenderer()->setOpaqueness(isOpaque);
}

void ParticleSystemController::showCheckboxToSelectWireframe(ParticleSystem* particleSystem)
{
	bool isWireframe = particleSystem->getRenderer()->isWireframe();
	ImGui::Checkbox("isWireframe", &isWireframe);
	particleSystem->getRenderer()->setWireframe(isWireframe);
}

void ParticleSystemController::showConfigWidgetsOfRenderer(RendererType rendererType)
{
	switch (rendererType)
	{
	case RendererType::Sprite:
	{
		
		break;
	}
	case RendererType::Ribbon:
	{

		break;
	}
	}
}
