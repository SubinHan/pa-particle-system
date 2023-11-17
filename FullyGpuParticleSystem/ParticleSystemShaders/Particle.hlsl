#ifndef PARTICLE_HLSL
#define PARTICLE_HLSL

struct Particle
{
	float3 Position;
	float InitialSize;

	float3 Velocity;
	float InitialLifetime;

	float3 Acceleration;
	float InitialOpacity;

	float3 InitialColor;
	float RemainLifetime;

	float3 EndColor;
	float EndSize;

	float EndOpacity;
	float SpawnTime;
	uint SpawnOrderInFrame;
	float DistanceFromPrevious;

	float DistanceFromStart;
	float3 Pad;
};

static const uint PARTICLECOUNTER_OFFSET_NEW_NUMALIVES = 0;
static const uint PARTICLECOUNTER_OFFSET_NUMALIVES = 4;
static const uint PARTICLECOUNTER_OFFSET_NUMSURVIVED = 8;
#endif