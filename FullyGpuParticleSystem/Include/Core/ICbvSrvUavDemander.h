#pragma once

#include "d3dx12.h"

class ICbvSrvUavDemander
{
public:
	virtual int getNumDescriptorsToDemand() const = 0;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) = 0;
};