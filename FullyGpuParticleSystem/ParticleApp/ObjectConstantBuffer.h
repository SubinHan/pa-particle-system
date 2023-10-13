#pragma once

#include "Util/MathHelper.h"
#include "Core/ICbvSrvUavDemander.h"
#include "Core/UploadBuffer.h"

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 WorldViewProj = MathHelper::identity4x4();
};

class ObjectConstantBuffer : public ICbvSrvUavDemander
{
public:
	ObjectConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device>);

	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;
	
	void CopyData(int elementIndex, const ObjectConstants& data);

private:
	std::unique_ptr<UploadBuffer<ObjectConstants>> _objectConstantBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> _device;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hCpu;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hGpu;
};
