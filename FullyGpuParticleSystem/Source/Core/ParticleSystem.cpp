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

#include "pix3.h"

#include <iostream>

constexpr double FIXED_DELTA_TIME = 1.0 / 120.0;

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

	ObjectConstants objConstants = {};
	
	DirectX::XMMATRIX world = XMLoadFloat4x4(&_world);

	XMStoreFloat4x4(&objConstants._world, XMMatrixTranspose(world));

	_deltaTimeAcc += gt.deltaTime();

	if (_deltaTimeAcc >= FIXED_DELTA_TIME)
	{
		PIXBeginEvent(commandList, PIX_COLOR(0, 255, 0), "Particle Emission");
		_emitter->emitParticles(
			commandList,
			objConstants,
			_deltaTimeAcc,
			gt.totalTime());
		PIXEndEvent(commandList);

		PIXBeginEvent(commandList, PIX_COLOR(0, 255, 0), "Particle Simulation");
		_simulator->simulateParticles(commandList, _deltaTimeAcc, gt.totalTime());
		PIXEndEvent(commandList);

		PIXBeginEvent(commandList, PIX_COLOR(0, 255, 0), "Particle Sort");
		if (_currentRendererType == RendererType::Ribbon)
		{
			_sorter->sortParticles(commandList);
		}
		PIXEndEvent(commandList);

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
