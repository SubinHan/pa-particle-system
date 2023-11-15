#include "Core/ParticleSorter.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/ParticlePreSorter.h"

using namespace Microsoft::WRL;

constexpr int SPAWN_ROOT_SLOT_CONSTANTS_BUFFER = 0;
constexpr int SPAWN_ROOT_SLOT_PARTICLES_BUFFER = SPAWN_ROOT_SLOT_CONSTANTS_BUFFER + 1;
constexpr int SPAWN_ROOT_SLOT_ALIVE_INDICES_BUFFER = SPAWN_ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int SPAWN_ROOT_SLOT_COUNTERS_BUFFER = SPAWN_ROOT_SLOT_ALIVE_INDICES_BUFFER + 1;

constexpr int PRE_ROOT_SLOT_CONSTANTS_BUFFER = 0;
constexpr int PRE_ROOT_SLOT_ALIVE_INDICES_BUFFER = PRE_ROOT_SLOT_CONSTANTS_BUFFER + 1;
constexpr int PRE_ROOT_SLOT_COUNTERS_BUFFER = PRE_ROOT_SLOT_ALIVE_INDICES_BUFFER + 1;


std::unique_ptr<ParticleSorter> ParticleSorter::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticleSorter>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleSorter::ParticleSorter(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name),
	_preSorter(ParticlePreSorter::create(resource, name + "pre"))
{
	buildShaders();
	setComputeShader(_shaderBySpawnOrder);

	auto& device = DxDevice::getInstance();
}

void ParticleSorter::sortParticles(ID3D12GraphicsCommandList* cmdList)
{
	_resource->uavBarrier(cmdList);
	_preSorter->preSortParticles(cmdList);

	readyDispatch(cmdList);
	const auto maxNumParticles = _resource->getEstimatedCurrentNumAliveParticlesAlignedPowerOfTwo();

	const auto numGroupsX = static_cast<UINT>(ceilf(maxNumParticles / 256.0f));
	const auto numGroupsY = 1;
	const auto numGroupsZ = 1;

	for (int sequenceSize = 2; sequenceSize <= maxNumParticles; sequenceSize <<= 1)
	{
		for (int stage = sequenceSize >> 1; stage > 0; stage >>= 1)
		{
			const SortConstants c =
			{
				maxNumParticles,
				sequenceSize,
				stage,
			};

			setConstants(cmdList, &c);
			_resource->uavBarrier(cmdList);
			cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
		}
	}
}

int ParticleSorter::getNum32BitsConstantsUsing()
{
	return sizeof(SortConstants) / 4;
}

void ParticleSorter::buildShaders()
{
	_shaderByIndex = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleSortByIndexCS.hlsl",
		nullptr,
		"BitonicSortCS",
		"cs_5_1");

	_shaderBySpawnOrder = DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleSortBySpawnOrderCS.hlsl",
		nullptr,
		"BitonicSortCS",
		"cs_5_1");
}