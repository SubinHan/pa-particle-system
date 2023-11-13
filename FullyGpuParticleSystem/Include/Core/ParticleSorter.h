#pragma once

#include "Core/ParticleComputePass.h"
#include "Core/ParticlePreSorter.h"
#include "Util/DxUtil.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class ParticleResource;
struct ObjectConstants;

struct SortConstants
{
	UINT ParticlesBufferSize;
	UINT SequenceSize;
	UINT Stage;
};

class ParticleSorter : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticleSorter> create(ParticleResource* resource, std::string name);

	ParticleSorter(ParticleResource* resource, std::string name);

	void sortParticles(
		ID3D12GraphicsCommandList* cmdList);

protected:
	virtual int getNum32BitsConstantsUsing() override;

private:
	void buildShaders();

private:
	std::unique_ptr<ParticlePreSorter> _preSorter;

	Microsoft::WRL::ComPtr<ID3DBlob> _shaderByIndex;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderBySpawnOrder;
};