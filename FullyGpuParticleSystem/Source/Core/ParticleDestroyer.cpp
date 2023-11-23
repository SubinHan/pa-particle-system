#include "Core/ParticleDestroyer.h"

#include "Core/ParticleResource.h"
#include "Core/ParticleAliveMover.h"
#include "Core/ParticlePostDestroyer.h"
#include "Util/MathHelper.h"

#include "pix3.h"

std::unique_ptr<ParticleDestroyer> ParticleDestroyer::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticleDestroyer>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleDestroyer::ParticleDestroyer(ParticleResource* resource, std::string name)
	: ParticleComputePass(resource, name),
	_aliveMover(ParticleAliveMover::create(resource, name + "aliveMover")),
	_postDestroyer(ParticlePostDestroyer::create(resource, name + "post"))
{
	setDefaultShader();
}

ParticleDestroyer::~ParticleDestroyer() = default;

void ParticleDestroyer::destroyExpiredParticles(
	ID3D12GraphicsCommandList* cmdList,
	double deltaTime, 
	float spawnRate, 
	float minLifetime,
	float maxLifetime)
{
	PIXBeginEvent(cmdList, PIX_COLOR(0, 255, 0), "Particle Destruction");

	float numMayBeExpired;
	if (MathHelper::nearlyZero(minLifetime))
	{
		numMayBeExpired = _resource->getMaxNumParticles();
	}
	else
	{
		static constexpr float CORRECTION = 1.5f;
		numMayBeExpired = static_cast<UINT>(ceilf(spawnRate * deltaTime * maxLifetime / minLifetime * CORRECTION));
	}

	ParticleDestroyerConstants c = 
	{ 
		numMayBeExpired,
		static_cast<float>(deltaTime),
	};

	readyDispatch(cmdList);
	setConstants(cmdList, &c);

	const auto numGroupsX = static_cast<UINT>(ceilf(numMayBeExpired / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	_resource->uavBarrier(cmdList);
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);

	_aliveMover->moveAlives(cmdList, numMayBeExpired, static_cast<float>(deltaTime));
	_postDestroyer->postDestroy(cmdList, numMayBeExpired);

	_resource->swapAliveIndicesBuffer();

	PIXEndEvent(cmdList);
}

bool ParticleDestroyer::needsStaticSampler()
{
	return false;
}

int ParticleDestroyer::getNum32BitsConstantsUsing()
{
	return sizeof(ParticleDestroyerConstants) / 4;
}

void ParticleDestroyer::setDefaultShader()
{
	setComputeShader(DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleDestroyCS.hlsl",
		nullptr,
		"DestroyCS",
		"cs_5_1"));
}
