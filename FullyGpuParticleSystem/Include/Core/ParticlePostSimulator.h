#pragma once

#include "Core/ParticleComputePass.h"

#include <memory>

class ParticlePostSimulator : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticlePostSimulator> create(ParticleResource* resource, std::string name);

	ParticlePostSimulator(ParticleResource* resource, std::string name);
	~ParticlePostSimulator();
	void postSimulate(ID3D12GraphicsCommandList* cmdList);

protected:
	virtual int getNum32BitsConstantsUsing() override;


private:
};