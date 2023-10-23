#include "Core/ParticleSystem.h"

#include "Core/DxDevice.h"
#include "Core/PassConstantBuffer.h"
#include "Core/ParticleResource.h"
#include "Core/ParticleEmitter.h"
#include "Core/ParticleSorter.h"
#include "Core/ParticleSimulator.h"
#include "Core/ParticlePass.h"
#include "Model/ObjectConstants.h"

#include <iostream>

ParticleSystem::ParticleSystem(DxDevice* device) :
	_device(device),
	_spawnRate(0.0f),
	_deltaTimeAfterSpawn(0.0f)
{
	init();
}

ParticleSystem::~ParticleSystem() = default;

int ParticleSystem::getNumDescriptorsToDemand() const
{
	return _resource->getNumDescriptorsToDemand();
}

void ParticleSystem::buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu)
{
	_resource->buildCbvSrvUav(hCpu, hGpu);
}

void ParticleSystem::onDraw(
	ID3D12GraphicsCommandList* commandList,
	const PassConstantBuffer& passCb, 
	const GameTimer& gt)
{
	ObjectConstants objConstants = {};
	
	DirectX::XMMATRIX world = XMLoadFloat4x4(&_world);

	XMStoreFloat4x4(&objConstants._world, XMMatrixTranspose(world));

	//D3D12_QUERY_HEAP_DESC heapDesc = {};
	//heapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
	//heapDesc.Count = 4;
	//heapDesc.NodeMask = 0;
	//ID3D12QueryHeap* pQueryHeap;
	//_device->getD3dDevice()->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&pQueryHeap));

	//commandList->EndQuery(pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 0);

	_deltaTimeAfterSpawn += gt.deltaTime();
	unsigned int numSpawn = 0;

	numSpawn = _deltaTimeAfterSpawn * _spawnRate;
	_deltaTimeAfterSpawn -= _spawnRateInv * static_cast<float>(numSpawn);

	if (numSpawn > 0)
	{
		_emitter->emitParticles(
			commandList,
			objConstants,
			numSpawn,
			gt.deltaTime());
	}

	//commandList->EndQuery(pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 1);
	_simulator->simulateParticles(commandList, gt.deltaTime());

	//_sorter->sortParticles(commandList);

	//commandList->EndQuery(pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 2);
	_pass->render(commandList, objConstants, passCb);

	//commandList->EndQuery(pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 3);

	//ID3D12Resource* pReadbackBuffer = nullptr;

	//auto heapPropertiesDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT64) * 4);

	//// Create the actual default buffer resource.
	//ThrowIfFailed(_device->getD3dDevice()->CreateCommittedResource(
	//	&heapPropertiesDefault,
	//	D3D12_HEAP_FLAG_NONE,
	//	&resourceDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&pReadbackBuffer)));

	//auto barrierCopy = CD3DX12_RESOURCE_BARRIER::Transition(
	//	pReadbackBuffer,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	D3D12_RESOURCE_STATE_COPY_DEST
	//);
	//commandList->ResourceBarrier(1, &barrierCopy);

	//commandList->ResolveQueryData(pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 0, 4, pReadbackBuffer, 0);

	//auto barrierRead = CD3DX12_RESOURCE_BARRIER::Transition(
	//	pReadbackBuffer,
	//	D3D12_RESOURCE_STATE_COPY_DEST,
	//	D3D12_RESOURCE_STATE_GENERIC_READ
	//);
	//commandList->ResourceBarrier(1, &barrierRead);

	//UINT64* pTimestamps;
	//ThrowIfFailed(pReadbackBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pTimestamps)));

	//UINT64 start = pTimestamps[0];
	//UINT64 afterEmit = pTimestamps[1];
	//UINT64 afterSimulate = pTimestamps[2];
	//UINT64 afterRender = pTimestamps[3];

	//auto cmdQueue = _device->getCommandQueue();
	//UINT64 frequency;
	//cmdQueue->GetTimestampFrequency(&frequency);
	//double time = static_cast<double>(afterRender - afterSimulate) / frequency;

	//std::cout << time << std::endl;
}

void ParticleSystem::setWorldTransform(const DirectX::XMFLOAT4X4& newWorldTransform)
{
	_world = newWorldTransform;
}

void ParticleSystem::setMaterial(Material* material)
{
	_pass->setMaterial(material);
}

void ParticleSystem::setSpawnRate(float spawnRate)
{
	_spawnRate = spawnRate;
	_spawnRateInv = 1.0f / _spawnRate;
}

void ParticleSystem::init()
{
	auto commandList = _device->startRecordingCommands();

	_resource = std::make_unique<ParticleResource>(
		_device->getD3dDevice(), 
		commandList.Get());
	_emitter = std::make_unique<ParticleEmitter>(
		_device->getD3dDevice(),
		_resource.get());
	_sorter = std::make_unique<ParticleSorter>(
		_device->getD3dDevice(),
		_resource.get());
	_simulator = std::make_unique<ParticleSimulator>(
		_device->getD3dDevice(),
		_resource.get());
	_pass = std::make_unique<ParticlePass>(
		_device,
		_resource.get());

	_device->submitCommands(commandList);
}
