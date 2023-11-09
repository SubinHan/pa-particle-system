#pragma once

#include "Core/ParticlePass.h"

class ParticleRenderPass : public ParticlePass
{
public:
	ParticleRenderPass(ParticleResource* resource, std::string name);

	void bindGraphicsResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot);

	void setVerticalShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setHullShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setDomainShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setGeometryShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setPixelShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderVs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderHs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderDs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderGs;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPs;

	bool _isShaderDirty;
};