#pragma once

#include "Core/ParticleComputePass.h"

#include <memory>

class ParticleResource;

struct PreSortConstants
{
	UINT fillEndIndex;
	UINT FillNumber;
};

class ParticlePreSorter : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticlePreSorter> create(ParticleResource* resource, std::string name);

	ParticlePreSorter(ParticleResource* resource, std::string name);
	~ParticlePreSorter();
	void preSortParticles(ID3D12GraphicsCommandList* cmdList);

protected:
	virtual int getNum32BitsConstantsUsing() override;
};