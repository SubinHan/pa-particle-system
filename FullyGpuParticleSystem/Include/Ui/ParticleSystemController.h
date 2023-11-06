#pragma once

#include "Ui/IWindow.h"

#include <string>
#include <vector>
#include <memory>

class ParticleSystem;
class ParticleSystemManager;

enum class RendererType;

class ParticleSystemController : public IWindow
{
public:
	ParticleSystemController(ParticleSystemManager* particleSystemManager);
	~ParticleSystemController();

	virtual void show() override;
	virtual bool isAlive() override;

private:
	void saveParticleSystemManager();
	void loadParticleSystemManager();

	ParticleSystem* createNewParticleSystem(std::string name);
	void deleteParticleSystemAndSaveFiles(int index);

	// rendere index is enum of RendererType.
	void showConfigWidgetsOfRenderer(RendererType rendererType);

private:
	ParticleSystemManager* _particleSystemManager;

	std::unique_ptr<IWindow> _showingWindow;

	std::vector<float> _spawnRate;

	int _expandedParticleSystem;
};