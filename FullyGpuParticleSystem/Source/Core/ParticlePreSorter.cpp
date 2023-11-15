#include "Core/ParticlePreSorter.h"

#include "Core/ParticleResource.h"

std::unique_ptr<ParticlePreSorter> ParticlePreSorter::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticlePreSorter>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticlePreSorter::ParticlePreSorter(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name)
{
	setComputeShader(DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticlePreSortCS0.hlsl",
		nullptr,
		"PreSortCS",
		"cs_5_1"));
}

ParticlePreSorter::~ParticlePreSorter() = default;

void ParticlePreSorter::preSortParticles(ID3D12GraphicsCommandList* cmdList)
{
	PreSortConstants c =
	{
		_resource->getReservedParticlesBufferSize(),
		0,
	};


	readyDispatch(cmdList);
	setConstants(cmdList, &c);

	const auto maxNumParticles = _resource->getReservedParticlesBufferSize();

	const auto numGroupsX = static_cast<UINT>(ceilf(maxNumParticles / 256.0f));
	const auto numGroupsY = 1;
	const auto numGroupsZ = 1;

	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
}

int ParticlePreSorter::getNum32BitsConstantsUsing()
{
	return sizeof(PreSortConstants) / 4;
}
