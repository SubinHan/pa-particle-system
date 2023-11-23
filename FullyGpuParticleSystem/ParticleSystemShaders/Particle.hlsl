#ifndef PARTICLE_HLSL
#define PARTICLE_HLSL

// packed.
struct Particle
{
	uint PositionXY;
	uint PositionZVelocityX;
	uint VelocityYZ;
	uint InitialLifetimeAndRemainLifetime;
	//----//
	uint AccelerationXY;
	uint AccelerationZAndSpawnTime;
	uint InitialSizeAndEndSize;
	uint DistanceFromPreviousAndDistanceFromStart;
	//----//
	uint InitialColor;
	uint EndColor;


};

//struct Particle
//{
//	float3 Position;
//	float InitialSize;
//	//----//
//	float3 Velocity;
//	float InitialLifetime;
//	//----//
//	float3 Acceleration;
//	float EndSize;
//	//----//
//	uint InitialColor;
//	uint EndColor;
//	float RemainLifetime;
//	float DistanceFromPrevious;
//	//----//
//	float DistanceFromStart;
//	float SpawnTime;
//};

static const uint PARTICLECOUNTER_OFFSET_NEW_NUMALIVES = 0;
static const uint PARTICLECOUNTER_OFFSET_NUMALIVES = 4;
static const uint PARTICLECOUNTER_OFFSET_NUMSURVIVED = 8;
#endif