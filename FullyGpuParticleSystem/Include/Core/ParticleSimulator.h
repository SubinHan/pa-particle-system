#pragma once

#include "Core/ParticleComputePass.h"
#include "Util/DxUtil.h"

#include <memory>
#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class ParticleResource;
class HlslGeneratorSimulate;
class ShaderStatementNode;
class ParticlePostSimulator;

struct ParticleSimulateConstants
{
	float DeltaTime;
};

class ParticleSimulator : public ParticleComputePass
{
public:
	static std::unique_ptr<ParticleSimulator> create(ParticleResource* resource, std::string name);

	ParticleSimulator(ParticleResource* resource, std::string name);
	~ParticleSimulator();
	void simulateParticles(ID3D12GraphicsCommandList* cmdList, float deltaTime);

protected:
	virtual bool needsStaticSampler() override;
	virtual int getNum32BitsConstantsUsing() override;

private:
	void setDefaultShader();

private:
	std::unique_ptr<ParticlePostSimulator> _postSimulator;
};