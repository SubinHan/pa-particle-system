#pragma once

#include "Util/MathHelper.h"
#include "Core/ICbvSrvUavDemander.h"
#include "Core/UploadBuffer.h"

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 InvView = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 Proj = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 InvProj = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 ViewProj = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 InvViewProj = MathHelper::identity4x4();
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;
};

class PassConstantBuffer : public ICbvSrvUavDemander
{
public:
	PassConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device>);

	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;
	
	void copyData(int elementIndex, const PassConstants& data);

    CD3DX12_CPU_DESCRIPTOR_HANDLE getCpuHandle();
    CD3DX12_GPU_DESCRIPTOR_HANDLE getGpuHandle();

private:
	std::unique_ptr<UploadBuffer<PassConstants>> _passConstantBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> _device;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hCpu;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hGpu;
};
