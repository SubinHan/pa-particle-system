#pragma once

#include "Util/MathHelper.h"

struct Particle
{
	DirectX::XMFLOAT3 Position;
	float InitialSize;

	DirectX::XMFLOAT3 Velocity;
	float InitialLifetime;

	DirectX::XMFLOAT3 Acceleration;
	float InitialOpacity;

	DirectX::XMFLOAT3 InitialColor;
	float RemainLifetime;

	DirectX::XMFLOAT3 EndColor;
	float EndSize;

	float EndOpacity;
	float SpawnTime;
	UINT SpawnOrderInFrame;
};