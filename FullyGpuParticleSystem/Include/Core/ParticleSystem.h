#pragma once

#include "Core/ICbvSrvUavDemander.h"
#include "Util/GameTimer.h"
#include "Util/MathHelper.h"

#include <memory>

enum class RendererType;

class DxDevice;
class ParticleResource;
class ParticleEmitter;
class ParticleSorter;
class ParticleDestroyer;
class ParticleSimulator;
class ParticleRenderPass;
class ParticleClearer;
class PassConstantBuffer;

struct ID3D12GraphicsCommandList;
struct Material;

class ParticleSystem : public ICbvSrvUavDemander
{
public:
	ParticleSystem(DxDevice* device, std::string name);
	~ParticleSystem();

	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;

	void onDraw(
		ID3D12GraphicsCommandList* commandList, 
		const PassConstantBuffer& passCb, 
		const GameTimer& gt);

	void setWorldTransform(const DirectX::XMFLOAT4X4& newWorldTransform);

	/// <summary>
	/// Set number of particles to spawn per a second.
	/// </summary>
	/// <param name="spawnRate"></param>
	void setSpawnRate(float spawnRate);

	RendererType getRendererType();
	void setRendererType(RendererType type);

	ParticleEmitter* getEmitter();
	ParticleDestroyer* getDestroyer();
	ParticleRenderPass* getRenderer();

	std::string getName();
	float getSpawnRate();

private:
	void init();
	void clearParticleSystem(ID3D12GraphicsCommandList* commandList);

private:
	DxDevice* _device;

	std::string _name;

	std::unique_ptr<ParticleResource> _resource;
	std::unique_ptr<ParticleEmitter> _emitter;
	//std::unique_ptr<ParticleSorter> _sorter;
	std::unique_ptr<ParticleDestroyer> _simulator;
	std::unique_ptr<ParticleRenderPass> _renderer;
	std::unique_ptr<ParticleClearer> _clearer;

	DirectX::XMFLOAT4X4 _world = MathHelper::identity4x4();

	double _deltaTimeAcc;

	RendererType _currentRendererType;

	bool _canDraw;
	bool _shouldClear;
};