#include "Core/ParticleDestroyer.h"

#include "Core/ParticleResource.h"
#include "Core/ParticleAliveMover.h"
#include "Core/ParticlePostDestroyer.h"
#include "Core/HlslGeneratorSimulate.h"
#include "Util/MathHelper.h"

#include "pix3.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";
static const std::wstring BASE_SIMULATOR_SHADER_PATH = L"ParticleSystemShaders/ParticleDestroyCS.hlsl";

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
	double totalTime,
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
		static_cast<float>(totalTime),
	};

	readyDispatch(cmdList);
	setConstants(cmdList, &c);

	const auto numGroupsX = static_cast<UINT>(ceilf(numMayBeExpired / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	_resource->uavBarrier(cmdList);
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);

	_aliveMover->moveAlives(cmdList, numMayBeExpired, static_cast<float>(deltaTime), static_cast<float>(totalTime));
	_postDestroyer->postDestroy(cmdList, numMayBeExpired);

	_resource->swapAliveIndicesBuffer();

	PIXEndEvent(cmdList);
}

ParticleAliveMover* ParticleDestroyer::getParticleAliveMover()
{
	return _aliveMover.get();
}

bool ParticleDestroyer::needsStaticSampler()
{
	return true;
}

int ParticleDestroyer::getNum32BitsConstantsUsing()
{
	return sizeof(ParticleDestroyerConstants) / 4;
}

void ParticleDestroyer::setDefaultShader()
{
	HlslGeneratorSimulate hlslGenerator(BASE_SIMULATOR_SHADER_PATH);

	UINT deltaTimeIndex = hlslGenerator.getDeltaTime();
	UINT positionIndex = hlslGenerator.getPosition();
	UINT randFloat3 = hlslGenerator.randFloat3();
	UINT minusHalf = hlslGenerator.newFloat3(-0.5f, -0.5f, -0.5f);
	UINT minusHalfToPlusHalf = hlslGenerator.addFloat3(randFloat3, minusHalf);
	UINT noisedPositionOffset = hlslGenerator.multiplyFloat3ByScalar(minusHalfToPlusHalf, deltaTimeIndex);
	UINT scaler = hlslGenerator.newFloat(5.0f);
	UINT noisedPositionOffsetScaled = hlslGenerator.multiplyFloat3ByScalar(noisedPositionOffset, scaler);
	UINT positionResult = hlslGenerator.addFloat3(positionIndex, noisedPositionOffsetScaled);

	hlslGenerator.setPosition(positionResult);

	hlslGenerator.generateShaderFile(SHADER_ROOT_PATH + L"temp.hlsl");
	setComputeShader(DxUtil::compileShader(SHADER_ROOT_PATH + L"temp.hlsl", nullptr, "DestroyCS", "cs_5_1"));
	setShaderStatementGraph(hlslGenerator.getShaderStatementGraph());
}
