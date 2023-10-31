#pragma once

#include "Core/ParticlePass.h"
#include "Util/DxUtil.h"

#include <memory>
#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class ParticleResource;
class HlslGeneratorSimulate;
class ShaderStatementNode;

struct ParticleSimulateConstants
{
	float DeltaTime;
};

class ParticleSimulator : public ParticlePass
{
public:
	ParticleSimulator(ParticleResource* resource, std::string name);
	~ParticleSimulator();
	void simulateParticles(ID3D12GraphicsCommandList* cmdList, float deltaTime);

protected:
	virtual std::vector<CD3DX12_ROOT_PARAMETER> buildRootParameter() override;
	virtual int getNumSrvUsing() override;
	virtual int getNumUavUsing() override;
	virtual bool needsStaticSampler() override;

	virtual void buildPsos() override;

private:
	void setDefaultShader();
	void buildPostSimulationShader();
	

private:
	static constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = 0;
	static constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
	static constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT = ROOT_SLOT_PARTICLES_BUFFER + 1;
	static constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK = ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT + 1;
	static constexpr int ROOT_SLOT_DEADS_INDICES_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK + 1;
	static constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_DEADS_INDICES_BUFFER + 1;

	std::unique_ptr<HlslGeneratorSimulate> _hlslGenerator;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoPost;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPost;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignaturePost;

	CD3DX12_DESCRIPTOR_RANGE _uavTable0;
	CD3DX12_DESCRIPTOR_RANGE _uavTable1;
};