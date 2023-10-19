#pragma once

#include "Util/MathHelper.h"

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 InvView = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 Proj = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 InvProj = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 ViewProj = MathHelper::identity4x4();
    DirectX::XMFLOAT4X4 InvViewProj = MathHelper::identity4x4();
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;
};