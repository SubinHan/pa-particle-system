#include "Particle.hlsl"

cbuffer cbUpdateConstants : register(b0)
{
	float deltaTime;
}

RWStructuredBuffer<Particle> particles		: register(u0);

RWStructuredBuffer<uint> aliveIndices		: register(u1);
RWStructuredBuffer<uint> newAliveIndices	: register(u2);
RWStructuredBuffer<uint> deadIndices		: register(u3);

RWByteAddressBuffer counters;				: register(u4);

// each thread updates a particle and kills if expired.
[numthreads(256, 1, 1)]
void UpdateCS(
	int3 groupThreadId : SV_GroupThreadID,
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	const int id = dispatchThreadId.x;
	uint numAlives = 
		counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	if (id >= numAlives)
	{
		return;
	}

	const int particleIndex = alivesIndices[id];
	Particle particle = particles[particleIndex];
	particle.Lifetime -= deltaTime;

	const bool isExpired = particle.Lifetime <= 0.f;

	if (isExpired)
	{
		// kill
		uint numDeads;
		conuters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMDEADS, 1, numDeads);

		deadIndices[numDeads] = particleIndex;
	}
	else
	{
		// TODO: Simulate particles.
		const float3 velocityBefore = particle.Velocity;
		particle.Velocity += particle.Acceleration;
		particle.Position += 
			(velocityBefore + particle.Velocity) * deltaTime * 0.5f;

		// move alive particles to ping-pong buffer 
		uint numNewAlives;
		counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES_POST_UPDATE, 1, numAlives);

		newAliveIndices[numAlives] = particleIndex;
	}
}