#include "Core/TextureManager.h"

#include "Core/DxDevice.h"
#include "Core/TextureBuffer.h"

#include <memory>

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager() = default;

TextureManager* TextureManager::getInstance()
{
	static std::unique_ptr<TextureManager> instance =
		std::unique_ptr<TextureManager>(new TextureManager());
	return instance.get();
}

void TextureManager::loadTexture(
	ID3D12GraphicsCommandList* commandList, 
	DxDevice* device, 
	std::string textureName,
	std::wstring texturePath)
{
	_textureBuffers[textureName] =
		std::move(std::make_unique<TextureBuffer>(
			texturePath,
			device,
			commandList
			));

	device->registerCbvSrvUavDescriptorDemander(_textureBuffers[textureName].get());
}

Texture TextureManager::getTexture(std::string name)
{
	return _textures[name];
}

CD3DX12_GPU_DESCRIPTOR_HANDLE TextureManager::getSrvGpuHandle(std::string textureName)
{
	//assert(_textureBuffers.count(textureName) > 0);
	return _textureBuffers[textureName]->getSrvGpuHandle();
}

