#pragma once

#include <memory>
#include <vector>
#include <string>

class DxDevice;
class ParticleSystem;

class ParticleSystemManager
{
public:
	ParticleSystemManager(DxDevice* device);
	~ParticleSystemManager();

	void createNewParticleSystem(const std::string name);

	ParticleSystem* getParticleSystemByIndex(const int index) const;
	ParticleSystem* getParticleSystemByName(const std::string name) const;

private:
	std::vector<std::unique_ptr<ParticleSystem>> _particleSystems;
};