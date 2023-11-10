#include "Core/ParticleSimulator.h"

#include "Core/DxDevice.h"
#include "Core/ParticleResource.h"
#include "Core/HlslGeneratorSimulate.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/Generated/";
static const std::wstring BASE_SIMULATOR_SHADER_PATH = L"ParticleSystemShaders/ParticleSimulateCSBase.hlsl";

using Microsoft::WRL::ComPtr;

ParticleSimulator::ParticleSimulator(ParticleResource* resource, std::string name) :
	ParticleComputePass(resource, name),
	_hlslGenerator(std::make_unique<HlslGeneratorSimulate>(BASE_SIMULATOR_SHADER_PATH))
{
	buildPostSimulationShader();
	setDefaultShader();
}

ParticleSimulator::~ParticleSimulator() = default;

void ParticleSimulator::simulateParticles(ID3D12GraphicsCommandList* cmdList, float deltaTime)
{
	ParticleSimulateConstants c = { deltaTime };

	cmdList->SetComputeRootSignature(_rootSignature.Get());

	cmdList->SetPipelineState(_pso.Get());

	cmdList->SetComputeRoot32BitConstants(ROOT_SLOT_PASS_CONSTANTS_BUFFER, 1, &c, 0);
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_PARTICLES_BUFFER, _resource->getParticlesResource()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT, _resource->getAliveIndicesResourceFront()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK, _resource->getAliveIndicesResourceBack()->GetGPUVirtualAddress());
	cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_DEADS_INDICES_BUFFER, _resource->getDeadIndicesResource()->GetGPUVirtualAddress());
	cmdList->SetComputeRootDescriptorTable(ROOT_SLOT_COUNTERS_BUFFER, _resource->getCountersUavGpuHandle());
	//cmdList->SetComputeRootUnorderedAccessView(ROOT_SLOT_COUNTERS_BUFFER, _resource->getCountersResource()->GetGPUVirtualAddress());
	
	bindComputeResourcesOfRegisteredNodes(cmdList, 6);

	const auto numGroupsX = static_cast<UINT>(ceilf(_resource->getMaxNumParticles() / 256.0f));
	const auto numGroupsY = 1;
	const UINT numGroupsZ = 1;
	cmdList->Dispatch(numGroupsX, numGroupsY, numGroupsZ);

	cmdList->SetComputeRootSignature(_rootSignaturePost.Get());
	cmdList->SetPipelineState(_psoPost.Get());
	cmdList->SetComputeRootDescriptorTable(0, _resource->getCountersUavGpuHandle());
	//cmdList->SetComputeRootUnorderedAccessView(0, _resource->getCountersResource()->GetGPUVirtualAddress());
	cmdList->Dispatch(1, 1, 1);

	_resource->swapAliveIndicesBuffer();
}

void ParticleSimulator::setDefaultShader()
{
	UINT deltaTimeIndex = _hlslGenerator->getDeltaTime();
	UINT positionIndex = _hlslGenerator->getPosition();
	UINT randFloat3 = _hlslGenerator->randFloat3();
	UINT minusHalf = _hlslGenerator->newFloat3(-0.5f, -0.5f, -0.5f);
	UINT minusHalfToPlusHalf = _hlslGenerator->addFloat3(randFloat3, minusHalf);
	UINT noisedPositionOffset = _hlslGenerator->multiplyFloat3ByScalar(minusHalfToPlusHalf, deltaTimeIndex);
	UINT scaler = _hlslGenerator->newFloat(5.0f);
	UINT noisedPositionOffsetScaled = _hlslGenerator->multiplyFloat3ByScalar(noisedPositionOffset, scaler);
	UINT positionResult = _hlslGenerator->addFloat3(positionIndex, noisedPositionOffsetScaled);

	_hlslGenerator->setPosition(positionResult);

	_hlslGenerator->generateShaderFile(SHADER_ROOT_PATH + L"temp.hlsl");
	setComputeShader(DxUtil::compileShader(SHADER_ROOT_PATH + L"temp.hlsl", nullptr, "SimulateCS", "cs_5_1"));
}

std::vector<CD3DX12_ROOT_PARAMETER> ParticleSimulator::buildRootParameter()
{
	std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter(6);

	slotRootParameter[ROOT_SLOT_PASS_CONSTANTS_BUFFER]
		.InitAsConstants(1, 0);
	slotRootParameter[ROOT_SLOT_PARTICLES_BUFFER]
		.InitAsUnorderedAccessView(0);
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT]
		.InitAsUnorderedAccessView(1);
	slotRootParameter[ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK]
		.InitAsUnorderedAccessView(2);
	slotRootParameter[ROOT_SLOT_DEADS_INDICES_BUFFER]
		.InitAsUnorderedAccessView(3);

	_uavTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 4);
	slotRootParameter[ROOT_SLOT_COUNTERS_BUFFER]
		.InitAsDescriptorTable(1, &_uavTable0);

	// build post
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[1];
		_uavTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		slotRootParameter[0]
			.InitAsDescriptorTable(1, &_uavTable1);

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

		auto& device = DxDevice::getInstance();

		ThrowIfFailed(device.getD3dDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(_rootSignaturePost.GetAddressOf())));
	}

	return slotRootParameter;
}

int ParticleSimulator::getNumSrvUsing()
{
	return 0;
}

int ParticleSimulator::getNumUavUsing()
{
	return 5;
}

bool ParticleSimulator::needsStaticSampler()
{
	return true;
}

void ParticleSimulator::buildPsos()
{
	auto device = DxDevice::getInstance().getD3dDevice();

	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = _rootSignature.Get();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shader->GetBufferPointer()),
		_shader->GetBufferSize()
	};
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(
		device->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_pso)));

	psoDesc.pRootSignature = _rootSignaturePost.Get();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shaderPost->GetBufferPointer()),
		_shaderPost->GetBufferSize()
	};
	ThrowIfFailed(
		device->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_psoPost)));
}

void ParticleSimulator::buildPostSimulationShader()
{
	_shaderPost = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticlePostSimulateCS.hlsl",
		nullptr,
		"PostSimulateCS",
		"cs_5_1");
}
