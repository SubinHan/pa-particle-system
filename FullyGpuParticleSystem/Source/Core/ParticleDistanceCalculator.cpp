#include "Core/ParticleDistanceCalculator.h"

#include "Core/ParticleResource.h"

std::unique_ptr<ParticleDistanceCalculator> ParticleDistanceCalculator::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticleDistanceCalculator>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleDistanceCalculator::ParticleDistanceCalculator(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name)
{
	setComputeShader(DxUtil::compileShader(
		L"ParticleSystemShaders\\ParticleRibbonDistanceCS.hlsl",
		nullptr,
		"BrentKung",
		"cs_5_1"));
}

ParticleDistanceCalculator::~ParticleDistanceCalculator() = default;

void ParticleDistanceCalculator::calculateRibbonDistanceFromStart(ID3D12GraphicsCommandList* cmdList)
{
	DistanceConstants c;

	UINT numParticles = _resource->getReservedParticlesBufferSize();

	UINT groupSizeX = static_cast<UINT>(ceil(static_cast<float>(_resource->getMaxNumParticles()) / 1024.0f)) / 2;

	readyDispatch(cmdList);

	c.IndexOffsetFrom = 1;
	c.IndexOffsetTo = 2;
	UINT shiftOffset = 0;
	// up sweep
	for (int i = numParticles >> 1; i > 0; i >>= 1)
	{
		c.NumWorkers = i;
		c.ShiftOffset = shiftOffset;
		setConstants(cmdList, &c);
		_resource->uavBarrier(cmdList);
		cmdList->Dispatch(groupSizeX, 1, 1);
		++shiftOffset;
	}

	--shiftOffset;

	c.IndexOffsetFrom = 2;
	c.IndexOffsetTo = 3;
	// down sweep
	for (int i = 2; i < numParticles; i <<= 1)
	{
		--shiftOffset;
		c.NumWorkers = i - 1;
		c.ShiftOffset = shiftOffset;
		setConstants(cmdList, &c);
		_resource->uavBarrier(cmdList);
		cmdList->Dispatch(groupSizeX, 1, 1);
	}
}

int ParticleDistanceCalculator::getNum32BitsConstantsUsing()
{
	return sizeof(DistanceConstants) / 4;
}
