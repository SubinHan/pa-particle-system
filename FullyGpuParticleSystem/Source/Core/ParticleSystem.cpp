#include "Core/ParticleSystem.h"

#include "Core/DxDevice.h"
#include "Core/PassConstantBuffer.h"
#include "Core/ParticleResource.h"
#include "Core/ParticleEmitter.h"
#include "Core/ParticleSorter.h"
#include "Core/ParticleDestroyer.h"
#include "Core/ParticleSimulator.h"
#include "Core/ParticleSpriteRenderer.h"
#include "Core/ParticleRibbonRenderer.h"
#include "Core/ParticleClearer.h"
#include "Model/ObjectConstants.h"
#include "Model/Material.h"
#include "Model/RendererType.h"

#include "pix3.h"

#include <iostream>

constexpr double FIXED_DELTA_TIME = 1.0 / 60.0;

ParticleSystem::ParticleSystem(DxDevice* device, std::string name) :
	_device(device),
	_name(name),
	_deltaTimeAcc(0.0)
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

	if (_shouldClear)
	{
		_shouldClear = false;
		clearParticleSystem(commandList);
	}

	ObjectConstants objConstants = {};
	
	DirectX::XMMATRIX world = XMLoadFloat4x4(&_world);

	XMStoreFloat4x4(&objConstants._world, XMMatrixTranspose(world));

	_deltaTimeAcc += gt.deltaTime();

	if (_deltaTimeAcc >= FIXED_DELTA_TIME)
	{
		_emitter->emitParticles(
			commandList,
			objConstants,
			_deltaTimeAcc,
			gt.totalTime());

		_simulator->destroyExpiredParticles(
			commandList,
			_deltaTimeAcc,
			gt.totalTime(),
			getSpawnRate(),
			_resource->getMinLifetimeOfParticles(),
			_resource->getMaxLifetimeOfParticles());

		_deltaTimeAcc = 0.0;
	}

	PIXBeginEvent(commandList, PIX_COLOR(0, 255, 0), "Particle Render");
	_renderer->render(commandList, objConstants, passCb);
	PIXEndEvent(commandList);
}

void ParticleSystem::setWorldTransform(const DirectX::XMFLOAT4X4& newWorldTransform)
{
	_world = newWorldTransform;
}

void ParticleSystem::setSpawnRate(float spawnRate)
{
	if (_emitter->getSpawnRate() == spawnRate)
		return;

	_emitter->setSpawnRate(spawnRate);
	_shouldClear = true;
}

RendererType ParticleSystem::getRendererType()
{
	return _currentRendererType;
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

ParticleDestroyer* ParticleSystem::getDestroyer()
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
	_simulator = ParticleDestroyer::create(_resource.get(), _name + "_Destroyer");
	_renderer = ParticleSpriteRenderer::create(_resource.get(), _name + "_Renderer");
	_clearer = ParticleClearer::create(_resource.get(), _name + "_Clearer");
}

void ParticleSystem::clearParticleSystem(ID3D12GraphicsCommandList* commandList)
{
	_deltaTimeAcc = 0;
	_clearer->clear(commandList);
}
