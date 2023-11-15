#pragma once

#include "Core/ParticleComputePass.h"

#include <memory>

class ParticleResource;

struct DistanceConstants
{
	UINT NumWorkers;
	UINT IndexOffsetFrom;
	UINT IndexOffsetTo;
	UINT ShiftOffset;
};

class ParticleDistanceCalculator : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticleDistanceCalculator> create(ParticleResource* resource, std::string name);

	ParticleDistanceCalculator(ParticleResource* resource, std::string name);
	virtual ~ParticleDistanceCalculator();

	void calculateRibbonDistanceFromStart(ID3D12GraphicsCommandList* cmdList);

protected:
	virtual int getNum32BitsConstantsUsing() override;
};