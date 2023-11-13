#include "Core/ParticlePostSimulator.h"



std::unique_ptr<ParticlePostSimulator> ParticlePostSimulator::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticlePostSimulator>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticlePostSimulator::ParticlePostSimulator(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name)
{
	setComputeShader(DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticlePostSimulateCS.hlsl",
		nullptr,
		"PostSimulateCS",
		"cs_5_1"));
}

ParticlePostSimulator::~ParticlePostSimulator() = default;

void ParticlePostSimulator::postSimulate(ID3D12GraphicsCommandList * cmdList)
{
	readyDispatch(cmdList);
	cmdList->Dispatch(1, 1, 1);
}

int ParticlePostSimulator::getNum32BitsConstantsUsing()
{
	return 0;
}