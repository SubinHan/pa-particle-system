#pragma once

#include "Core/ICbvSrvUavDemander.h"
#include "Core/Hashable.h"
#include "Util/DxUtil.h"

#include <memory>
#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

class ParticleResource;
class HlslGeneratorSimulate;

struct ParticleSimulateConstants
{
	float DeltaTime;
};

class ParticleSimulator : public Hashable
{
public:
	ParticleSimulator(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource, std::string name);
	~ParticleSimulator();

	std::string getName();

	ID3D12RootSignature* getRootSignature();
	ID3DBlob* getShader();
	ID3D12PipelineState* getPipelineStateObject();

	void simulateParticles(ID3D12GraphicsCommandList* cmdList, float deltaTime);

	void compileShader();
	void setShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);

private:
	void buildRootSignature();
	void buildShaders();
	void buildPsos();

private:
	static constexpr int ROOT_SLOT_PASS_CONSTANTS_BUFFER = 0;
	static constexpr int ROOT_SLOT_PARTICLES_BUFFER = ROOT_SLOT_PASS_CONSTANTS_BUFFER + 1;
	static constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT = ROOT_SLOT_PARTICLES_BUFFER + 1;
	static constexpr int ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK = ROOT_SLOT_ALIVES_INDICES_BUFFER_FRONT + 1;
	static constexpr int ROOT_SLOT_DEADS_INDICES_BUFFER = ROOT_SLOT_ALIVES_INDICES_BUFFER_BACK + 1;
	static constexpr int ROOT_SLOT_COUNTERS_BUFFER = ROOT_SLOT_DEADS_INDICES_BUFFER + 1;

	Microsoft::WRL::ComPtr<ID3D12Device> _device;
	ParticleResource* _resource;

	std::string _name;

	std::unique_ptr<HlslGeneratorSimulate> _hlslGenerator;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;
	Microsoft::WRL::ComPtr<ID3DBlob> _shader;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _psoPost;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPost;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignaturePost;

};