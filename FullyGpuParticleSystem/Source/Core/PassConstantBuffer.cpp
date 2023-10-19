#include "Core/PassConstantBuffer.h"

#include "Core/UploadBuffer.h"

#include <memory>

constexpr int NUM_DESCRIPTOR_TO_DEMAND = 1;

using namespace Microsoft::WRL;

PassConstantBuffer::PassConstantBuffer(ComPtr<ID3D12Device> device) :
	_device(device)
{
}

int PassConstantBuffer::getNumDescriptorsToDemand() const
{
	return NUM_DESCRIPTOR_TO_DEMAND;
}

void PassConstantBuffer::buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu)
{
	_hCpu = hCpu;
	_hGpu = hGpu;

	_passConstantBuffer =
		std::make_unique<UploadBuffer<PassConstants>>(
			_device.Get(), 1, true);

	UINT passConstantBufferByteSize =
		DxUtil::calcConstantBufferByteSize(sizeof(PassConstants));

	D3D12_GPU_VIRTUAL_ADDRESS constantBufferAddress =
		_passConstantBuffer->Resource()->GetGPUVirtualAddress();

	int boxConstantBufferIndex = 0;
	constantBufferAddress += boxConstantBufferIndex * passConstantBufferByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constantBufferAddress;
	cbvDesc.SizeInBytes = passConstantBufferByteSize;

	_device->CreateConstantBufferView(
		&cbvDesc,
		hCpu);
}

void PassConstantBuffer::copyData(int elementIndex, const PassConstants& data)
{
	_passConstantBuffer->copyData(elementIndex, data);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE PassConstantBuffer::getCpuHandle() const
{
	return _hCpu;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE PassConstantBuffer::getGpuHandle() const
{
	return _hGpu;
}
