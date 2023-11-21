#pragma once

#include "Util/MathHelper.h"

struct Particle
{
	UINT PositionXY;
	UINT PositionZVelocityX;
	UINT VelocityYZ;
	UINT InitialLifetimeAndRemainLifetime;
	//----//
	UINT AccelerationXY;
	UINT AccelerationZAndSpawnTime;
	UINT InitialSizeAndEndSize;
	UINT DistanceFromPreviousAndDistanceFromStart;
	//----//
	UINT InitialColor;
	UINT EndColor;
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