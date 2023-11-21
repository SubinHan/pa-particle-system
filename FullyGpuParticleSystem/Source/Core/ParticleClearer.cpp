#include "Core/ParticleClearer.h"

#include "Core/ParticleResource.h"

std::unique_ptr<ParticleClearer> ParticleClearer::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticleClearer>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleClearer::ParticleClearer(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name)
{
	setDefaultShader();
}

ParticleClearer::~ParticleClearer() = default;

void ParticleClearer::clear(ID3D12GraphicsCommandList* cmdList)
{
	readyDispatch(cmdList);
	_resource->uavBarrier(cmdList);
	cmdList->Dispatch(1, 1, 1);
}

bool ParticleClearer::needsStaticSampler()
{
	return false;
}

int ParticleClearer::getNum32BitsConstantsUsing()
{
	return 0;
}

void ParticleClearer::setDefaultShader()
{
	setComputeShader(DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleClearCS.hlsl",
		nullptr,
		"ClearCS",
		"cs_5_1"));
}
