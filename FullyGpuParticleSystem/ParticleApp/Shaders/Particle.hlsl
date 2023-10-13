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

static const uint PARTICLECOUNTER_OFFSET_NUMDEADS = 0;
static const uint PARTICLECOUNTER_OFFSET_NUMALIVES = 4;
static const uint PARTICLECOUNTER_OFFSET_NUMALIVES_POST_UPDATE = 8;