#include "Core/ParticleSimulator.h"

#include "Core/ParticleResource.h"
#include "Core/HlslTranslatorSimulate.h"
#include "Util/DxDebug.h"

#include "d3dx12.h"

static const std::wstring SHADER_ROOT_PATH = L"ParticleSystemShaders/";
static const std::wstring BASE_SIMULATOR_SHADER_PATH = L"ParticleSystemShaders/ParticleSimulateCSBase.hlsl";

using Microsoft::WRL::ComPtr;

ParticleSimulator::ParticleSimulator(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource) :
	_device(device),
	_resource(resource),
	_hlslTranslator(std::make_unique<HlslTranslatorSimulate>(BASE_SIMULATOR_SHADER_PATH))
{
	buildRootSignature();
	buildShaders();
	buildPsos();
}

ParticleSimulator::~ParticleSimulator() = default;

ID3D12RootSignature* ParticleSimulator::getRootSignature()
{
	return _rootSignature.Get();
}

ID3DBlob* ParticleSimulator::getShader()
{
	return _shader.Get();
}

ID3D12PipelineState* ParticleSimulator::getPipelineStateObject()
{
	return _pso.Get();
}

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

void ParticleSimulator::compileShader()
{
	const std::wstring shaderPath = SHADER_ROOT_PATH + std::to_wstring(_hash) + L".hlsl";

	_hlslTranslator->compile(shaderPath);

	_shader = DxUtil::compileShader(
		shaderPath,
		nullptr,
		"SimulateCS",
		"cs_5_1");

	_shaderPost = DxUtil::compileShader(
		L"ParticleApp\\Shaders\\ParticlePostSimulateCS.hlsl",
		nullptr,
		"PostSimulateCS",
		"cs_5_1");
}

void ParticleSimulator::buildRootSignature()
{
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[6];

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

		CD3DX12_DESCRIPTOR_RANGE uavTable;
		uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 4);
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

	// build post

	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[1];
		CD3DX12_DESCRIPTOR_RANGE uavTable;
		uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		slotRootParameter[0]
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
			IID_PPV_ARGS(_rootSignaturePost.GetAddressOf())));
	}
}

void ParticleSimulator::buildShaders()
{
	UINT deltaTimeIndex = _hlslTranslator->getDeltaTime();
	UINT positionIndex = _hlslTranslator->getPositionAfterSimulation();
	UINT randFloat3 = _hlslTranslator->randFloat3();
	UINT minusHalf = _hlslTranslator->newFloat3(-0.5f, -0.5f, -0.5f);
	UINT minusHalfToPlusHalf = _hlslTranslator->addFloat3(randFloat3, minusHalf);
	UINT noisedPositionOffset = _hlslTranslator->multiplyFloat3ByScalar(minusHalfToPlusHalf, deltaTimeIndex);
	UINT scaler = _hlslTranslator->newFloat1(5.0f);
	UINT noisedPositionOffsetScaled = _hlslTranslator->multiplyFloat3ByScalar(noisedPositionOffset, scaler);
	UINT positionResult = _hlslTranslator->addFloat3(positionIndex, noisedPositionOffsetScaled);

	_hlslTranslator->setPosition(positionResult);

	compileShader();
}

void ParticleSimulator::buildPsos()
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

	psoDesc.pRootSignature = _rootSignaturePost.Get();
	psoDesc.CS =
	{
		reinterpret_cast<BYTE*>(_shaderPost->GetBufferPointer()),
		_shaderPost->GetBufferSize()
	};
	ThrowIfFailed(
		_device->CreateComputePipelineState(
			&psoDesc, IID_PPV_ARGS(&_psoPost)));
}
