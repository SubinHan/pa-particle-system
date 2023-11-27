#include "Core/ParticleAliveMover.h"

#include "Core/ParticleResource.h"
#include "Core/ParticleDestroyer.h"
#include "Core/HlslGeneratorSimulate.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";
static const std::wstring BASE_SIMULATOR_SHADER_PATH = L"ParticleSystemShaders/ParticleMoveAlivesCS.hlsl";

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

void ParticleAliveMover::moveAlives(
	ID3D12GraphicsCommandList* cmdList,
	UINT numMayBeExpired,
	float deltaTime,
	float totalTime)
{
	ParticleDestroyerConstants c = 
	{
		numMayBeExpired,
		deltaTime,
		totalTime,
	};

	readyDispatch(cmdList, true);
	setConstants(cmdList, &c);

	const auto numGroupsX = static_cast<UINT>(ceilf(_resource->getEstimatedCurrentNumAliveParticles() / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	_resource->uavBarrier(cmdList);
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
	_resource->transitCountersCbvToUav(cmdList);
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
	setComputeShader(DxUtil::compileShader(SHADER_ROOT_PATH + L"temp.hlsl", nullptr, "MoveAlivesCS", "cs_5_1"));
	setShaderStatementGraph(hlslGenerator.getShaderStatementGraph());
}
