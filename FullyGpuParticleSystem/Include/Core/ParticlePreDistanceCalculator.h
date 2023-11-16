#pragma once

#include "Core/ParticleComputePass.h"

#include <memory>

class ParticleResource;

class ParticlePreDistanceCalculator : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticlePreDistanceCalculator> create(ParticleResource* resource, std::string name);

	ParticlePreDistanceCalculator(ParticleResource* resource, std::string name);
	virtual ~ParticlePreDistanceCalculator();

	void preRibbonDistance(ID3D12GraphicsCommandList* cmdList);

protected:
	virtual int getNum32BitsConstantsUsing() override;
};