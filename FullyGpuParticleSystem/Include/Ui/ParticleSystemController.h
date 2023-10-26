#pragma once

#include "Ui/IWindow.h"

#include <string>
#include <vector>
#include <memory>

class ParticleSystem;
class ParticleSystemManager;

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

private:
	ParticleSystemManager* _particleSystemManager;

	std::vector<std::unique_ptr<IWindow>> _showingWindow;

	std::vector<float> _spawnRate;
};