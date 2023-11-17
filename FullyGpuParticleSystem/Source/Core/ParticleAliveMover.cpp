#include "Core/ParticleAliveMover.h"

#include "Core/ParticleResource.h"
#include "Core/ParticleDestroyer.h"

std::unique_ptr<ParticleAliveMover> ParticleAliveMover::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticleAliveMover>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleAliveMover::ParticleAliveMover(ParticleResource* resource, std::string name)
	: ParticleComputePass(resource, name)
{
	setDefaultShader();
}

ParticleAliveMover::~ParticleAliveMover() = default;

void ParticleAliveMover::moveAlives(ID3D12GraphicsCommandList* cmdList, UINT numMayBeExpired, float deltaTime)
{
	ParticleDestroyerConstants c = 
	{
		numMayBeExpired,
		deltaTime,
	};

	readyDispatch(cmdList);
	setConstants(cmdList, &c);

	const auto numGroupsX = static_cast<UINT>(ceilf(_resource->getEstimatedCurrentNumAliveParticles() / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	_resource->uavBarrier(cmdList);
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
}

bool ParticleAliveMover::needsStaticSampler()
{
	return false;
}

int ParticleAliveMover::getNum32BitsConstantsUsing()
{
	return sizeof(ParticleDestroyerConstants) / 4;
}

void ParticleAliveMover::setDefaultShader()
{
	setComputeShader(DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleMoveAlivesCS.hlsl",
		nullptr,
		"MoveAlivesCS",
		"cs_5_1"));
}
