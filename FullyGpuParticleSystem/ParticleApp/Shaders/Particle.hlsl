#pragma once

struct Particle
{
	float3 Position;
	float Size;

	float3 Velocity;
	float Lifetime;

	float3 Acceleration;
	float Opacity;

	float3 Color;
};