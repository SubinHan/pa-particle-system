#pragma once

#include "Core/ICbvSrvUavDemander.h"

class DxDevice;

class ImguiInitializer : public ICbvSrvUavDemander
{
public:
	ImguiInitializer(DxDevice* device, HWND hwnd);


	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;

private:
	DxDevice* _device;
	HWND _hwnd;
};