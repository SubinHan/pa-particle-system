#include "Core/ParticlePostDestroyer.h"

#include "Core/ParticleResource.h"

std::unique_ptr<ParticlePostDestroyer> ParticlePostDestroyer::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticlePostDestroyer>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticlePostDestroyer::ParticlePostDestroyer(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name)
{
	setDefaultShader();
}

ParticlePostDestroyer::~ParticlePostDestroyer() = default;

void ParticlePostDestroyer::postDestroy(ID3D12GraphicsCommandList* cmdList, UINT numMayBeExpired)
{
	UINT c = numMayBeExpired;

	readyDispatch(cmdList);
	setConstants(cmdList, &c);
	_resource->uavBarrier(cmdList);
	cmdList->Dispatch(1, 1, 1);
}

bool ParticlePostDestroyer::needsStaticSampler()
{
	return false;
}

int ParticlePostDestroyer::getNum32BitsConstantsUsing()
{
	return 1;
}

void ParticlePostDestroyer::setDefaultShader()
{
	setComputeShader(DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticlePostDestroyCS.hlsl",
		nullptr,
		"PostDestroyCS",
		"cs_5_1"));
}
