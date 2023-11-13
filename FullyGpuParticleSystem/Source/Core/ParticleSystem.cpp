#include "Core/ParticleSystem.h"

#include "Core/DxDevice.h"
#include "Core/PassConstantBuffer.h"
#include "Core/ParticleResource.h"
#include "Core/ParticleEmitter.h"
#include "Core/ParticleSorter.h"
#include "Core/ParticleSimulator.h"
#include "Core/ParticleSpriteRenderer.h"
#include "Core/ParticleRibbonRenderer.h"
#include "Model/ObjectConstants.h"
#include "Model/Material.h"
#include "Model/RendererType.h"

#include <iostream>

ParticleSystem::ParticleSystem(DxDevice* device, std::string name) :
	_device(device),
	_name(name),
	_spawnRate(100.0f),
	_spawnRateInv(1.0f / _spawnRate),
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

	_canDraw = true;
}

void ParticleSystem::onDraw(
	ID3D12GraphicsCommandList* commandList,
	const PassConstantBuffer& passCb, 
	const GameTimer& gt)
{
	if (!_canDraw)
		return;

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

	if (numSpawn > 0)
	{
		_deltaTimeAfterSpawn -= _spawnRateInv * static_cast<float>(numSpawn);
		_deltaTimeAfterSpawn = max(0.0f, _deltaTimeAfterSpawn);

		_emitter->emitParticles(
			commandList,
			objConstants,
			gt);
	}

	//commandList->EndQuery(pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 1);
	_simulator->simulateParticles(commandList, gt.deltaTime());

	if (_currentRendererType == RendererType::Ribbon)
	{
		_sorter->sortParticles(commandList);
	}

	//commandList->EndQuery(pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, 2);
	_renderer->render(commandList, objConstants, passCb);

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

void ParticleSystem::setSpawnRate(float spawnRate)
{
	_emitter->setSpawnRate(spawnRate);
}

void ParticleSystem::setRendererType(RendererType type)
{
	if (_currentRendererType == type)
		return;

	_currentRendererType = type;
	auto graph = _renderer->getShaderStatementGraph();

	switch (_currentRendererType)
	{
	case RendererType::Sprite:
	{
		_renderer = ParticleSpriteRenderer::create(
			_resource.get(),
			_name + "_Renderer");
		break;
	}
	case RendererType::Ribbon:
	{
		_renderer = ParticleRibbonRenderer::create(
			_resource.get(),
			_name + "_Renderer");
		break;
	}
	default:
	{
		assert(0 && "Unknown RendererType was given.");
		break;
	}
	}

	_renderer->setShaderStatementGraph(graph);
}

ParticleEmitter* ParticleSystem::getEmitter()
{
	return _emitter.get();
}

ParticleSimulator* ParticleSystem::getSimulator()
{
	return _simulator.get();
}

ParticleRenderPass* ParticleSystem::getRenderer()
{
	return _renderer.get();
}

std::string ParticleSystem::getName()
{
	return _name;
}

float ParticleSystem::getSpawnRate()
{
	return _emitter->getSpawnRate();
}

void ParticleSystem::init()
{
	auto commandList = _device->getCommandList();

	_resource = ParticleResource::create(commandList.Get());
	_emitter = ParticleEmitter::create(_resource.get(), _name + "_Emitter");
	_sorter = ParticleSorter::create(_resource.get(), _name + "_Sorter");
	_simulator = ParticleSimulator::create(_resource.get(), _name + "_Simulator");
	_renderer = ParticleSpriteRenderer::create(_resource.get(), _name + "_Renderer");

	//_device->submitCommands(commandList);
}
