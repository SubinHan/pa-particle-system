#pragma once

#include "Core/ParticleComputePass.h"

class ParticleClearer : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticleClearer> create(ParticleResource* resource, std::string name);

	ParticleClearer(ParticleResource* resource, std::string name);
	virtual ~ParticleClearer();
	void clear(ID3D12GraphicsCommandList* cmdList);

protected:
	virtual bool needsStaticSampler() override;
	virtual int getNum32BitsConstantsUsing() override;

private:
	void setDefaultShader();

private:
};