#include "Core/ParticlePreDistanceCalculator.h"

#include "Core/ParticleResource.h"

std::unique_ptr<ParticlePreDistanceCalculator> ParticlePreDistanceCalculator::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticlePreDistanceCalculator>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticlePreDistanceCalculator::ParticlePreDistanceCalculator(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name)
{
	setComputeShader(DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticlePreRibbonDistanceCS.hlsl",
		nullptr,
		"PreRibbonDistanceCS",
		"cs_5_1"));
}

ParticlePreDistanceCalculator::~ParticlePreDistanceCalculator() = default;

void ParticlePreDistanceCalculator::preRibbonDistance(ID3D12GraphicsCommandList* cmdList)
{
	UINT groupSizeX = static_cast<UINT>(ceil(static_cast<float>(_resource->getMaxNumParticles()) / 256.0f));

	readyDispatch(cmdList);
	_resource->uavBarrier(cmdList);
	cmdList->Dispatch(groupSizeX, 1, 1);
}

int ParticlePreDistanceCalculator::getNum32BitsConstantsUsing()
{
	return 0;
}
