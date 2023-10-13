#include "ObjectConstantBuffer.h"
#include "Core/UploadBuffer.h"
#include <memory>

constexpr int NUM_DESCRIPTOR_TO_DEMAND = 1;

using namespace Microsoft::WRL;

ObjectConstantBuffer::ObjectConstantBuffer(ComPtr<ID3D12Device> device) :
	_device(device)
{
}

int ObjectConstantBuffer::getNumDescriptorsToDemand() const
{
	return NUM_DESCRIPTOR_TO_DEMAND;
}

void ObjectConstantBuffer::buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu)
{
	_hCpu = hCpu;
	_hGpu = hGpu;

	_objectConstantBuffer =
		std::make_unique<UploadBuffer<ObjectConstants>>(
			_device.Get(), 1, true);

	UINT objectConstantBufferByteSize =
		DxUtil::calcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS constantBufferAddress =
		_objectConstantBuffer->Resource()->GetGPUVirtualAddress();

	int boxConstantBufferIndex = 0;
	constantBufferAddress += boxConstantBufferIndex * objectConstantBufferByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constantBufferAddress;
	cbvDesc.SizeInBytes = objectConstantBufferByteSize;

	_device->CreateConstantBufferView(
		&cbvDesc,
		hCpu);
}

void ObjectConstantBuffer::CopyData(int elementIndex, const ObjectConstants& data)
{
	_objectConstantBuffer->CopyData(elementIndex, data);
}
