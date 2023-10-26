#pragma once

#include "Model/Texture.h"
#include "Util/DxUtil.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class DxDevice;
class TextureBuffer;

struct ID3D12GraphicsCommandList;

class TextureManager
{

public:
	~TextureManager();

	static TextureManager* getInstance();

	void loadTexture(
		ID3D12GraphicsCommandList* commandList, 
		DxDevice* device,
		std::string textureName,
		std::wstring texturePath);
	Texture getTexture(std::string name);

	CD3DX12_GPU_DESCRIPTOR_HANDLE getSrvGpuHandle(std::string textureName);

private:
	TextureManager();

private:
	std::unordered_map<std::string, std::unique_ptr<TextureBuffer>> _textureBuffers;
	std::unordered_map<std::string, Texture> _textures;
};