#pragma once

#include "Core/ParticleComputePass.h"
#include "Util/DxUtil.h"
#include "Util/GameTimer.h"

#include <memory>
#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class ParticleResource;
class HlslGeneratorEmit;
struct ObjectConstants;

struct EmitConstants
{
	DirectX::XMFLOAT4X4 World;

	UINT EmitCount;
	DirectX::XMFLOAT3 Position;

	DirectX::XMFLOAT2 Orientation;
	float DeltaTime;
	UINT MaxNumParticles;

	float TotalTime;
};

class ParticleEmitter : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticleEmitter> create(ParticleResource* resource, std::string name);

	ParticleEmitter(ParticleResource* resource, std::string name);
	virtual ~ParticleEmitter();

	void setSpawnRate(float spawnRate);
	float getSpawnRate();
	void emitParticles(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const GameTimer& gt);

protected:
	virtual int getNum32BitsConstantsUsing() override;
	virtual void onShaderStatementGraphChanged() override;

private:
	void initDefault();
	void setDefaultShader();

private:
	float _spawnRate;
	float _spawnRateInv;
	float _deltaTimeAfterSpawn;

	float _averageLifetime;
};