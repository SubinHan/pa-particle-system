#pragma once

#include "ICbvSrvUavDemander.h"

#include <string>

class DxDevice;
struct Texture;

class TextureBuffer : public ICbvSrvUavDemander
{
public:
	TextureBuffer(
		std::wstring ddsFilePath,
		DxDevice* device,
		ID3D12GraphicsCommandList* commandList);

	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;

	CD3DX12_GPU_DESCRIPTOR_HANDLE getSrvGpuHandle();

private:
	DxDevice* _device;

	std::unique_ptr<Texture> _texture;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hCpu;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hGpu;
};