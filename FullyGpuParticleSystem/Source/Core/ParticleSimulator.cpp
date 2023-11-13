#include "Core/ParticleSimulator.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/HlslGeneratorSimulate.h"
#include "Core/ParticlePostSimulator.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";
static const std::wstring BASE_SIMULATOR_SHADER_PATH = L"ParticleSystemShaders/ParticleSimulateCSBase.hlsl";

using Microsoft::WRL::ComPtr;

std::unique_ptr<ParticleSimulator> ParticleSimulator::create(ParticleResource* resource, std::string name)
{
	auto created =
		std::make_unique<ParticleSimulator>(resource, name);

	created->buildRootSignature();

	return std::move(created);
}

ParticleSimulator::ParticleSimulator(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name),
	_postSimulator(ParticlePostSimulator::create(resource, name + "post"))
{
	setDefaultShader();
}

ParticleSimulator::~ParticleSimulator() = default;

void ParticleSimulator::simulateParticles(ID3D12GraphicsCommandList* cmdList, float deltaTime)
{
	ParticleSimulateConstants c = { deltaTime };

	readyDispatch(cmdList);
	setConstants(cmdList, &c);

	const auto numGroupsX = static_cast<UINT>(ceilf(_resource->getMaxNumParticles() / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);

	_postSimulator->postSimulate(cmdList);

	_resource->swapAliveIndicesBuffer();
}

void ParticleSimulator::setDefaultShader()
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
	setComputeShader(DxUtil::compileShader(SHADER_ROOT_PATH + L"temp.hlsl", nullptr, "SimulateCS", "cs_5_1"));
	setShaderStatementGraph(hlslGenerator.getShaderStatementGraph());
}

bool ParticleSimulator::needsStaticSampler()
{
	return true;
}

int ParticleSimulator::getNum32BitsConstantsUsing()
{
	return sizeof(ParticleSimulateConstants) / 4;
}
