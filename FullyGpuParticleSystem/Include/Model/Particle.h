#pragma once

#include "Util/MathHelper.h"

struct Particle
{
	DirectX::XMFLOAT3 Position;
	float Size;

	DirectX::XMFLOAT3 Velocity;
	float Lifetime;

	DirectX::XMFLOAT3 Acceleration;
	float Opacity;

	DirectX::XMFLOAT3 Color;
};