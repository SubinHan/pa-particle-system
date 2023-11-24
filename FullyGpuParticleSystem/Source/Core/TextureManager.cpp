#include "Core/TextureManager.h"

#include "Core/DxDevice.h"
#include "Core/TextureBuffer.h"
#include "Core/TextureTrimmer.h"

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
	_textureNames.push_back(textureName);

	_textureBuffers[textureName] =
		std::move(std::make_unique<TextureBuffer>(
			texturePath,
			device,
			commandList
			));
	
	std::string texturePathString;
	texturePathString.assign(texturePath.begin(), texturePath.end());
	texturePathString.replace(texturePathString.find("dds"), 3, "png");

	_textureTrimmers[textureName] =
		std::move(std::make_unique<TextureTrimmer>(
			texturePathString));

	device->registerCbvSrvUavDescriptorDemander(_textureBuffers[textureName].get());
}

std::vector<std::string> TextureManager::getTextureNames()
{
	return _textureNames;
}

Texture TextureManager::getTexture(std::string name)
{
	return _textures[name];
}

std::vector<DirectX::XMFLOAT2> TextureManager::getBoundingPolygon(std::string textureName, int k)
{
	return _textureTrimmers[textureName]->getBoundingPolygon(k);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE TextureManager::getSrvGpuHandle(std::string textureName)
{
	//assert(_textureBuffers.count(textureName) > 0);
	return _textureBuffers[textureName]->getSrvGpuHandle();
}

