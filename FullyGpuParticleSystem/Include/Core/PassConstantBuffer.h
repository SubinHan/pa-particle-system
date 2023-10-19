#pragma once

#include "Model/PassConstants.h"
#include "Util/MathHelper.h"
#include "Core/ICbvSrvUavDemander.h"
#include "Core/UploadBuffer.h"

class PassConstantBuffer : public ICbvSrvUavDemander
{
public:
	PassConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device>);

	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;
	
	void copyData(int elementIndex, const PassConstants& data);

    CD3DX12_CPU_DESCRIPTOR_HANDLE getCpuHandle() const;
    CD3DX12_GPU_DESCRIPTOR_HANDLE getGpuHandle() const;

private:
	std::unique_ptr<UploadBuffer<PassConstants>> _passConstantBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> _device;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hCpu;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hGpu;
};
