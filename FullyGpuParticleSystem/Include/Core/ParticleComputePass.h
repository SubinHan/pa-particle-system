#pragma once

#include "Core/ParticlePass.h"

class ParticleComputePass : public ParticlePass
{
public:
	ParticleComputePass(ParticleResource* resource, std::string name);

	void bindComputeResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot);

	void setComputeShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);

private:
	Microsoft::WRL::ComPtr<ID3DBlob> _computeShader;

	bool _isShaderDirty;
};