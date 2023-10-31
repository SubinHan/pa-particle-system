#pragma once

#include "ResourceViewType.h"

#include <string>
#include <d3dx12.h>

struct ID3D12Resource;

struct ResourceRequest
{
	ID3D12Resource* resource;
	ResourceViewType type;
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu;
};