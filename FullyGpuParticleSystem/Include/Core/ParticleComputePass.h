#pragma once

#include "Core/ParticlePass.h"

class ParticleComputePass : public ParticlePass
{
public:
	void bindComputeResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot);

	void setComputeShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
};