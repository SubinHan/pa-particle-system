#pragma once

#include "Core/ParticleComputePass.h"

class ParticleAliveMover : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticleAliveMover> create(ParticleResource* resource, std::string name);

	ParticleAliveMover(ParticleResource* resource, std::string name);
	virtual ~ParticleAliveMover();
	void moveAlives(
		ID3D12GraphicsCommandList* cmdList,
		UINT numMayBeExpired,
		float deltaTime,
		float totalTime);

protected:
	virtual bool needsStaticSampler() override;
	virtual int getNum32BitsConstantsUsing() override;

private:
	void setDefaultShader();

private:
};