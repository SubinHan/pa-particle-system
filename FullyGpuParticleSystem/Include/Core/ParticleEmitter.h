#pragma once

#include "Core/Hashable.h"
#include "Util/DxUtil.h"

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
};

class ParticleEmitter : public Hashable
{
public:
	ParticleEmitter(Microsoft::WRL::ComPtr<ID3D12Device> device, ParticleResource* resource, std::string name);
	virtual ~ParticleEmitter();

	std::string getName();

	ID3D12RootSignature* getRootSignature();
	ID3DBlob* getShader();
	ID3D12PipelineState* getPipelineStateObject();

	void emitParticles(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		int numParticlesToEmit, 
		float deltaTime);

	void compileShaders();
	void setShaderPs(Microsoft::WRL::ComPtr<ID3DBlob> shader);

private:
	void buildRootSignature();
	void buildShaders();
	void buildPsos();

private:
	Microsoft::WRL::ComPtr<ID3D12Device> _device;
	ParticleResource* _resource;

	std::string _name;

	std::unique_ptr<HlslGeneratorEmit> _hlslGenerator;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;
	Microsoft::WRL::ComPtr<ID3DBlob> _shader;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
};