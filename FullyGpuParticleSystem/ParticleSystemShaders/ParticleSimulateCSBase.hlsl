#include "ParticleApp/Shaders/Particle.hlsl"
#include "ParticleApp/Shaders/Util.hlsl"

cbuffer cbUpdateConstants : register(b0)
{
	float DeltaTime;
}

RWStructuredBuffer<Particle> particles		: register(u0);

RWStructuredBuffer<uint> aliveIndices		: register(u1);
RWStructuredBuffer<uint> newAliveIndices	: register(u2);
RWStructuredBuffer<uint> deadIndices		: register(u3);

RWByteAddressBuffer counters				: register(u4);

// each thread updates a particle and kills if expired.
[numthreads(256, 1, 1)]
void SimulateCS(
	int3 groupThreadId : SV_GroupThreadID,
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	const int id = dispatchThreadId.x;

	uint numAlives = 
		counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	if (uint(id) < numAlives)
	{
		const int particleIndex = aliveIndices[id];
		particles[particleIndex].Lifetime -= DeltaTime;

		const bool isExpired = particles[particleIndex].Lifetime <= 0.f;

		if (isExpired)
		{
			// kill
			uint numDeads;
			counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMDEADS, 1, numDeads);

			deadIndices[numDeads] = particleIndex;
		}
		else
		{
			float3 currentVelocity = particles[particleIndex].Velocity;
			float3 currentPosition = particles[particleIndex].Position;
			float3 currentAcceleration = particles[particleIndex].Acceleration;

			%s

			particles[particleIndex].Velocity += currentAcceleration * DeltaTime;
			particles[particleIndex].Position +=
				(currentVelocity + particles[particleIndex].Velocity) * DeltaTime * 0.5f;

			// move alive particles to ping-pong buffer 
			uint newIndex;
			counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES_POST_UPDATE, 1, newIndex);

			newAliveIndices[newIndex] = particleIndex;
		}
	}
}