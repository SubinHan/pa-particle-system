#pragma once

#include <wrl.h>
#include <string>

struct ID3D12Resource;

struct Texture
{
	// Unique material name for lookup.
	std::string Name;

	std::wstring Filename;

	UINT SrvIndex;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};
