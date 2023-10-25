#pragma once

#include "Ui/IWindow.h"

#include <vector>
#include <memory>

class ParticleSystem;

class ParticleSystemController : public IWindow
{
public:
	ParticleSystemController(std::vector<std::unique_ptr<ParticleSystem>>* particleSystems);
	~ParticleSystemController();

	virtual void show() override;
	virtual bool isAlive() override;

private:
	std::vector<std::unique_ptr<ParticleSystem>>* _particleSystems;

	std::vector<std::unique_ptr<IWindow>> _showingWindow;
};