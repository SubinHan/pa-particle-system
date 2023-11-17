#pragma once

#include "Core/ParticleComputePass.h"

class ParticlePostDestroyer : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticlePostDestroyer> create(ParticleResource* resource, std::string name);

	ParticlePostDestroyer(ParticleResource* resource, std::string name);
	virtual ~ParticlePostDestroyer();
	void postDestroy(ID3D12GraphicsCommandList* cmdList);

protected:
	virtual bool needsStaticSampler() override;
	virtual int getNum32BitsConstantsUsing() override;

private:
	void setDefaultShader();

private:
};