#pragma once

#include "Model/Texture.h"
#include "Util/DxUtil.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class DxDevice;
class TextureBuffer;
class TextureTrimmer;

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

	std::vector<std::string> getTextureNames();
	Texture getTexture(std::string name);

	// returns bounding k-gon of texture named given parameter.
	std::vector<DirectX::XMFLOAT2> getBoundingPolygon(std::string textureName, int k);

	CD3DX12_GPU_DESCRIPTOR_HANDLE getSrvGpuHandle(std::string textureName);

private:
	TextureManager();

private:
	std::unordered_map<std::string, std::unique_ptr<TextureBuffer>> _textureBuffers;
	std::unordered_map<std::string, Texture> _textures;
	std::vector<std::string> _textureNames;
	std::unordered_map<std::string, std::unique_ptr<TextureTrimmer>> _textureTrimmers;
};