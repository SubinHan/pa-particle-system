#include "Core/ParticleEmitter.h"

#include "Core/ParticleResource.h"
#include "Core/HlslGeneratorEmit.h"
#include "Model/ObjectConstants.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/";
static const std::wstring BASE_EMITTER_SHADER_PATH = L"ParticleSystemShaders/ParticleEmitCSBase.hlsl";

using Microsoft::WRL::ComPtr;

constexpr int ROOT_SLOT_OBJECT_CONSTANTS_BUFFER = 0;
constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = ROOT_SLOT_OBJECT_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER = ROOT_SLOT_PARTICLES_BUFFER + 1;
constexpr int ROOT_SLOT_DEADS_INDICES_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER + 1;
constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_DEADS_INDICES_BUFFER + 1;

ParticleEmitter::ParticleEmitter(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource) :
	Hashable(),
	_device(device),
	_resource(resource),
	_hlslGenerator(std::make_unique<HlslGeneratorEmit>(BASE_EMITTER_SHADER_PATH))
{
	buildRootSignature();
	buildShaders();
	buildPsos();
}

ParticleEmitter::~ParticleEmitter() = default;

ID3D12RootSignature* ParticleEmitter::getRootSignature()
{
	return _rootSignature.Get();
}

ID3DBlob* ParticleEmitter::getShader()
{
	return _shader.Get();
}

ID3D12PipelineState* ParticleEmitter::getPipelineStateObject()
{
	return _pso.Get();
}

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

void ParticleEmitter::compileShaders()
{
	const std::wstring shaderPath = SHADER_ROOT_PATH + std::to_wstring(_hash) + L".hlsl";

	_hlslGenerator->compile(shaderPath);

	_shader = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"EmitCS",
		"cs_5_1");
}

void ParticleEmitter::setShader(Microsoft::WRL::ComPtr<ID3DBlob> shader)
{
	_shader = shader;
	buildPsos();
}

void ParticleEmitter::buildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[6];

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

	CD3DX12_DESCRIPTOR_RANGE uavTable;
	uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);
	slotRootParameter[ROOT_SLOT_COUNTERS_BUFFER]
		.InitAsDescriptorTable(1, &uavTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		_countof(slotRootParameter),
		slotRootParameter,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_NONE);


	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(_device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.GetAddressOf())));

}

void ParticleEmitter::buildShaders()
{
	UINT positionIndex = _hlslGenerator->newFloat3(0.0f, 0.0f, 0.0f);
	UINT velocityIndex = _hlslGenerator->randFloat3();
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

	compileShaders();
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
	ThrowIfFailed(
		_device->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_pso)));
}
