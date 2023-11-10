#pragma once

#include "Core/ParticleComputePass.h"
#include "Util/DxUtil.h"
#include "Util/GameTimer.h"

#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class ParticleResource;
class HlslGeneratorEmit;
struct ObjectConstants;

struct EmitConstants
{
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
	ParticleEmitter(ParticleResource* resource, std::string name);
	virtual ~ParticleEmitter();

	void emitParticles(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		int numParticlesToEmit, 
		const GameTimer& gt);

protected:
	virtual std::vector<CD3DX12_ROOT_PARAMETER> buildRootParameter() override;
	virtual int getNumSrvUsing() override;
	virtual int getNumUavUsing() override;
	virtual bool needsStaticSampler() override;

	virtual void buildPsos() override;

private:
	void initDefault();
	void setDefaultShader();

	std::unique_ptr<HlslGeneratorEmit> _hlslGenerator;

	CD3DX12_DESCRIPTOR_RANGE _uavTable;
};