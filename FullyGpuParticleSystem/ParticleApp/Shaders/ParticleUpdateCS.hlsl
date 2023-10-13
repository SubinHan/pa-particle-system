#pragma once

#include "Particle.hlsl"

cbuffer cbUpdateConstants : register(b0)
{
	float deltaTime;
}

RWStructuredBuffer<Particle> particles;

RWStructuredBuffer<uint> aliveIndices;
RWStructuredBuffer<uint> newAliveIndices;
RWStructuredBuffer<uint> deadIndices;

RWByteAddressBuffer counters;

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