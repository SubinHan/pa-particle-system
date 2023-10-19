#pragma once

#include "Util/MathHelper.h"

#include "d3dx12.h"
#include <string>

struct Material
{
	// Unique material name for lookup.
	std::string Name;

	// Index into constant buffer corresponding to this material.
	int MatCBIndex = -1;

	// gpu handle for diffuse map texture.
	CD3DX12_GPU_DESCRIPTOR_HANDLE DiffuseSrvHandle;

	// gpu handle for normal map texture.
	CD3DX12_GPU_DESCRIPTOR_HANDLE NormalSrvHandle;

	// gpu handle for displacement map texture.
	CD3DX12_GPU_DESCRIPTOR_HANDLE DisplacementSrvHandle;

	// Material constant buffer data used for shading.
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = .25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::identity4x4();
};