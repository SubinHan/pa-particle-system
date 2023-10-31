#include "Core/ParticleEmitter.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/HlslGeneratorEmit.h"
#include "Model/ObjectConstants.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";
static const std::wstring BASE_EMITTER_SHADER_PATH = L"ParticleSystemShaders/ParticleEmitCSBase.hlsl";

using Microsoft::WRL::ComPtr;

constexpr int ROOT_SLOT_OBJECT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = ROOT_SLOT_OBJECT_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_DEADS_INDICES_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER + 1;
constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_DEADS_INDICES_BUFFER + 1;

ParticleEmitter::ParticleEmitter(ParticleResource* resource, std::string name) :
	ParticlePass(resource, name),
	_hlslGenerator(std::make_unique<HlslGeneratorEmit>(BASE_EMITTER_SHADER_PATH))
{
	initDefault();
}

ParticleEmitter::~ParticleEmitter() = default;

void ParticleEmitter::emitParticles(
	ID3D12GraphicsCommandList* cmdList,
	const ObjectConstants& objectConstants,
	int numParticlesToEmit, 
	float deltaTime)
{
	EmitConstants c = { 
		numParticlesToEmit, 
		DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f}, 
		DirectX::XMFLOAT2{0.0f, 0.0f},
		deltaTime,
		_resource->getMaxNumParticles()};

	cmdList->SetComputeRootSignature(_rootSignature.Get());

	cmdList->SetPipelineState(_pso.Get());

	cmdList->SetComputeRoot32BitConstants(ROOT_SLOT_OBJECT_CONSTANTS_BUFFER, sizeof(ObjectConstants) / 4, &objectConstants, 0);
	cmdList->SetComputeRoot32BitConstants(ROOT_SLOT_PASS_CONSTANTS_BUFFER, sizeof(EmitConstants) / 4, &c, 0);
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_PARTICLES_BUFFER, _resource->getParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_ALIVES_INDICES_BUFFER, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_DEADS_INDICES_BUFFER, _resource->getDeadIndicesResource()->GetGPUVirtualAddress());
	cmdList->SetComputeRootDescriptorTable(ROOT_SLOT_COUNTERS_BUFFER, _resource->getCountersUavGpuHandle());

	const auto numGroupsX = static_cast<UINT>(ceilf(numParticlesToEmit / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);
}

std::vector<CD3DX12_ROOT_PARAMETER> ParticleEmitter::buildRootParameter()
{
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(6);

	slotRootParameter[ROOT_SLOT_OBJECT_CONSTANTS_BUFFER]
		.InitAsConstants(sizeof(ObjectConstants) / 4, 0);
	slotRootParameter[ROOT_SLOT_PASS_CONSTANTS_BUFFER]
		.InitAsConstants(sizeof(EmitConstants) / 4, 1);
	slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER]
		.InitAsUnorderedAccessView(0);
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER]
		.InitAsUnorderedAccessView(1);
	slotRootParameter[ROOT_SLOT_DEADS_INDICES_BUFFER]
		.InitAsUnorderedAccessView(2);

	_uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);
	slotRootParameter[ROOT_SLOT_COUNTERS_BUFFER]
		.InitAsDescriptorTable(1, &_uavTable);

	return slotRootParameter;
}

int ParticleEmitter::getNumSrvUsing()
{
	return 0;
}

int ParticleEmitter::getNumUavUsing()
{
	return 3;
}

bool ParticleEmitter::needsStaticSampler()
{
	return false;
}

void ParticleEmitter::buildPsos()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = _rootSignature.Get();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shader->GetBufferPointer()),
		_shader->GetBufferSize()
	};
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	auto device = DxDevice::getInstance().getD3dDevice();
	ThrowIfFailed(
		device->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_pso)));
}

void ParticleEmitter::initDefault()
{
	setDefaultShader();
}

void ParticleEmitter::setDefaultShader()
{
	UINT positionIndex = _hlslGenerator->newFloat3(0.0f, 0.0f, 0.0f);

	UINT randFloat3 = _hlslGenerator->randFloat3();
	UINT minusHalfFloat3 = _hlslGenerator->newFloat3(-0.5f, -0.5f, -0.5f);
	UINT velocityIndex = _hlslGenerator->addFloat3(randFloat3, minusHalfFloat3);

	UINT accelerationIndex = _hlslGenerator->newFloat3(0.0f, -1.0f, 0.0f);

	UINT lifetimeIndex = _hlslGenerator->newFloat(4.0f);

	UINT sizeIndex = _hlslGenerator->newFloat(0.05f);

	UINT opacityIndex = _hlslGenerator->newFloat(1.0f);

	_hlslGenerator->setInitialPosition(positionIndex);
	_hlslGenerator->setInitialVelocity(velocityIndex);
	_hlslGenerator->setInitialAcceleration(accelerationIndex);
	_hlslGenerator->setInitialLifetime(lifetimeIndex);
	_hlslGenerator->setInitialSize(sizeIndex);
	_hlslGenerator->setInitialOpacity(opacityIndex);

	_hlslGenerator->generateShaderFile(SHADER_ROOT_PATH + L"temp.hlsl");
	setShader(DxUtil::compileShader(SHADER_ROOT_PATH + L"temp.hlsl", nullptr, "EmitCS", "cs_5_1"));
}
