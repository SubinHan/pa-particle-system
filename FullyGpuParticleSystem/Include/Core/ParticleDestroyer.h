#pragma once

#include "Core/ParticleComputePass.h"

struct ParticleDestroyerConstants
{
	UINT NumMayBeExpired;
	float DeltaTime;
	float TotalTime;
};

class ParticleAliveMover;
class ParticlePostDestroyer;

class ParticleDestroyer : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticleDestroyer> create(ParticleResource* resource, std::string name);

	ParticleDestroyer(ParticleResource* resource, std::string name);
	virtual ~ParticleDestroyer();
	void destroyExpiredParticles(
		ID3D12GraphicsCommandList* cmdList,
		double deltaTime,
		double totalTime,
		float spawnRate,
		float minLifetime,
		float maxLifetime);

	ParticleAliveMover* getParticleAliveMover();

protected:
	virtual bool needsStaticSampler() override;
	virtual int getNum32BitsConstantsUsing() override;

private:
	void setDefaultShader();

private:
	std::unique_ptr<ParticleAliveMover> _aliveMover;
	std::unique_ptr<ParticlePostDestroyer> _postDestroyer;
};