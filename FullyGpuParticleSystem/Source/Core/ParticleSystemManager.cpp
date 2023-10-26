#include "Core/ParticleSystemManager.h"

#include "Core/DxDevice.h"
#include "Core/ParticleSystem.h"

ParticleSystemManager::ParticleSystemManager(DxDevice* device) :
	_device(device)
{
}

ParticleSystemManager::~ParticleSystemManager() = default;

void ParticleSystemManager::createNewParticleSystem(const std::string name)
{
	auto particleSystem = std::make_unique<ParticleSystem>(_device, name);
	_device->registerCbvSrvUavDescriptorDemander(particleSystem.get());
	_particleSystems.push_back(std::move(particleSystem));
}

void ParticleSystemManager::removeParticleSystemByIndex(const int index)
{
	assert(index < _particleSystems.size() && "removeParticleSystemByIndex(): index out of ounds");

	_particleSystems.erase(_particleSystems.begin() + index);
}

void ParticleSystemManager::removeParticleSystemByName(const std::string name)
{
	for (int i = 0; i < _particleSystems.size(); ++i)
	{
		if (_particleSystems[i]->getName() == name)
			_particleSystems.erase(_particleSystems.begin() + i);
	}
}

ParticleSystem* ParticleSystemManager::getParticleSystemByIndex(const int index) const
{
	return _particleSystems[index].get();
}

ParticleSystem* ParticleSystemManager::getParticleSystemByName(const std::string name) const
{
	for (int i = 0; i < _particleSystems.size(); ++i)
	{
		if (_particleSystems[i]->getName() == name)
			return _particleSystems[i].get();
	}

	return nullptr;
}

int ParticleSystemManager::getNumParticleSystems()
{
	return _particleSystems.size();
}
