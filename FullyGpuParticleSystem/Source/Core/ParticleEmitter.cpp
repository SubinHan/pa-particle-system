#include "Core/ParticleEmitter.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/HlslGeneratorEmit.h"
#include "Core/ShaderStatementGraph.h"
#include "Core/ShaderStatementNode/ShaderStatementNodeSetValueByVariableName.h"
#include "Model/ObjectConstants.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"
#include <tuple>

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";
static const std::wstring BASE_EMITTER_SHADER_PATH = L"ParticleSystemShaders/ParticleEmitCSBase.hlsl";

using Microsoft::WRL::ComPtr;

constexpr int ROOT_SLOT_OBJECT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = ROOT_SLOT_OBJECT_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_DEADS_INDICES_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER + 1;
constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_DEADS_INDICES_BUFFER + 1;

std::unique_ptr<ParticleEmitter> ParticleEmitter::create(
	ParticleResource* resource,
	std::string name)
{
	auto created =
		std::make_unique<ParticleEmitter>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleEmitter::ParticleEmitter(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name),
	_spawnRate(100.0f),
	_spawnRateInv(1.0f / _spawnRate),
	_deltaTimeAfterSpawn(0.0f)
{
	initDefault();
}

ParticleEmitter::~ParticleEmitter() = default;

void ParticleEmitter::setSpawnRate(float spawnRate)
{
	_spawnRate = spawnRate;
	_spawnRateInv = 1.0 / _spawnRate;
	_resource->onEmittingPolicyChanged(_spawnRate, _averageLifetime);
}

float ParticleEmitter::getSpawnRate()
{
	return _spawnRate;
}

void ParticleEmitter::emitParticles(
	ID3D12GraphicsCommandList* cmdList,
	const ObjectConstants& objectConstants,
	const double deltaTime,
	const double totalTime)
{
	_deltaTimeAfterSpawn += deltaTime;
	unsigned int numSpawn = 0;

	numSpawn = _deltaTimeAfterSpawn * _spawnRate;

	if (numSpawn == 0)
		return;

	_deltaTimeAfterSpawn -= _spawnRateInv * static_cast<double>(numSpawn);
	_deltaTimeAfterSpawn = max(0.0, _deltaTimeAfterSpawn);

	EmitConstants c = {
		objectConstants._world,
		numSpawn,
		DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f}, 
		DirectX::XMFLOAT2{0.0f, 0.0f},
		deltaTime,
		_resource->getMaxNumParticles(),
		totalTime,
	};

	readyDispatch(cmdList);
	setConstants(cmdList, &c);

	const auto numGroupsX = static_cast<UINT>(ceilf(numSpawn / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	_resource->uavBarrier(cmdList);
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
}

int ParticleEmitter::getNum32BitsConstantsUsing()
{
	return sizeof(EmitConstants) / 4;
}

decltype(auto) findAverageLifetime(std::shared_ptr<ShaderStatementGraph> graph)
{
	float averageLifetime = 5.0f;
	float minLifetime = 5.0f;
	float maxLifetime = 5.0f;

	for (int i = 0; i < graph->getSize(); ++i)
	{
		auto maybeLifetimeNode =
			std::dynamic_pointer_cast<ShaderStatementNodeSetValueByVariableName>(
				graph->getNode(i));

		if (!maybeLifetimeNode)
			continue;

		if (maybeLifetimeNode->getVariableNameToSet() != "initialLifetime")
			continue;
		averageLifetime = maybeLifetimeNode->getEvaluatedFloat();
		minLifetime = maybeLifetimeNode->getEvaluatedFloatMin();
		maxLifetime = maybeLifetimeNode->getEvaluatedFloatMax();

		return std::make_tuple<>(averageLifetime, minLifetime, maxLifetime);
	}

	return std::make_tuple<>(averageLifetime, minLifetime, maxLifetime);
}

void ParticleEmitter::onShaderStatementGraphChanged()
{
	auto [averageLifetime, minLifetime, maxLifetime] = 
		findAverageLifetime(getShaderStatementGraph());

	_averageLifetime = averageLifetime;
	_minLifetime = minLifetime;
	_maxLifetime = maxLifetime;

	_resource->onEmittingPolicyChanged(
		_spawnRate,
		_averageLifetime, 
		_minLifetime,
		_maxLifetime);
}

void ParticleEmitter::initDefault()
{
	setDefaultShader();
}

void ParticleEmitter::setDefaultShader()
{
	HlslGeneratorEmit hlslGenerator(BASE_EMITTER_SHADER_PATH);

	UINT positionIndex = hlslGenerator.newFloat3(0.0f, 0.0f, 0.0f);

	UINT randFloat3 = hlslGenerator.randFloat3();
	UINT minusHalfFloat3 = hlslGenerator.newFloat3(-0.5f, -0.5f, -0.5f);
	UINT velocityIndex = hlslGenerator.addFloat3(randFloat3, minusHalfFloat3);

	UINT accelerationIndex = hlslGenerator.newFloat3(0.0f, -1.0f, 0.0f);

	UINT lifetimeIndex = hlslGenerator.newFloat(4.0f);

	UINT sizeIndex = hlslGenerator.newFloat(0.05f);

	UINT opacityIndex = hlslGenerator.newFloat(1.0f);

	hlslGenerator.setInitialPosition(positionIndex);
	hlslGenerator.setInitialVelocity(velocityIndex);
	hlslGenerator.setInitialAcceleration(accelerationIndex);
	hlslGenerator.setInitialLifetime(lifetimeIndex);
	hlslGenerator.setInitialSize(sizeIndex);
	hlslGenerator.setInitialOpacity(opacityIndex);

	hlslGenerator.generateShaderFile(SHADER_ROOT_PATH + L"temp.hlsl");
	setComputeShader(DxUtil::compileShader(SHADER_ROOT_PATH + L"temp.hlsl", nullptr, "EmitCS", "cs_5_1"));
	setShaderStatementGraph(hlslGenerator.getShaderStatementGraph());
}
