#pragma once

#include "Core/ICbvSrvUavDemander.h"
#include "Util/GameTimer.h"
#include "Util/MathHelper.h"

#include <memory>

class DxDevice;
class ParticleResource;
class ParticleEmitter;
class ParticleSorter;
class ParticleSimulator;
class ParticleRenderer;
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
	void setMaterial(Material* material);
	
	/// <summary>
	/// Set number of particles to spawn per a second.
	/// </summary>
	/// <param name="spawnRate"></param>
	void setSpawnRate(float spawnRate);

	ParticleEmitter* getEmitter();
	ParticleSimulator* getSimulator();
	ParticleRenderer* getRenderer();

	std::string getName();

private:
	void init();

private:
	DxDevice* _device;

	std::string _name;

	std::unique_ptr<ParticleResource> _resource;
	std::unique_ptr<ParticleEmitter> _emitter;
	std::unique_ptr<ParticleSorter> _sorter;
	std::unique_ptr<ParticleSimulator> _simulator;
	std::unique_ptr<ParticleRenderer> _renderer;

	DirectX::XMFLOAT4X4 _world = MathHelper::identity4x4();
	float _spawnRate;
	float _spawnRateInv;
	float _deltaTimeAfterSpawn;
};