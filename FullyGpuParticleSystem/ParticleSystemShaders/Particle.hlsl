#ifndef PARTICLE_HLSL
#define PARTICLE_HLSL

struct Particle
{
	min16float3 Position;
	min16float InitialSize;

	min16float3 Velocity;
	min16float InitialLifetime;

	min16float3 Acceleration;
	min16float EndSize;

	uint InitialColor;
	uint EndColor;

	min16float RemainLifetime;
	min16float DistanceFromPrevious;
	min16float DistanceFromStart;
	min16float SpawnTime;
};

static const uint PARTICLECOUNTER_OFFSET_NEW_NUMALIVES = 0;
static const uint PARTICLECOUNTER_OFFSET_NUMALIVES = 4;
static const uint PARTICLECOUNTER_OFFSET_NUMSURVIVED = 8;
#endif