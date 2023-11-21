#pragma once

#include "Util/MathHelper.h"

struct Particle
{
	DirectX::XMFLOAT4 PositionAndInitialSize;

	DirectX::XMFLOAT4 VelocityAndInitialLifetime;

	DirectX::XMFLOAT4 AccelerationAndEndSize;

	UINT InitialColor;
	UINT EndColor;
	float RemainLifetimeAndDistanceFromPrevious;
	float DistanceFromStartAndSpawnTime;
};

//struct Particle
//{
//	DirectX::XMFLOAT3 Position;
//	float InitialSize;
//
//	DirectX::XMFLOAT3 Velocity;
//	float InitialLifetime;
//	
//	DirectX::XMFLOAT3 Acceleration;
//	float EndSize;
//
//	UINT InitialColor;
//	UINT EndColor;
//	float RemainLifetime;
//	float DistanceFromPrevious;
//
//	float DistanceFromStart;
//	float SpawnTime;
//};