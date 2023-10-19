#include "Core/TextureBuffer.h"

#include "Core/DxDevice.h"
#include "Model/Texture.h"
#include "Util/DxDebug.h"
#include "Util/DDSTextureLoader.h"

TextureBuffer::TextureBuffer(
	std::wstring ddsFilePath, 
	DxDevice* device, 
	ID3D12GraphicsCommandList* commandList) :
	_device(device)
{
	_texture = std::make_unique<Texture>();
	_texture->Filename = ddsFilePath;

	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		_device->getD3dDevice().Get(),
		commandList,
		_texture->Filename.c_str(),
		_texture->Resource,
		_texture->UploadHeap));
}

int TextureBuffer::getNumDescriptorsToDemand() const
{
	return 1;
}

void TextureBuffer::buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu)
{
	_hCpu = hCpu;
	_hGpu = hGpu;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = _texture->Resource->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = _texture->Resource->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	_device->getD3dDevice()->CreateShaderResourceView(
		_texture->Resource.Get(), &srvDesc, hCpu);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE TextureBuffer::getSrvGpuHandle()
{
	return _hGpu;
}
