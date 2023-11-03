#include "ParticleSystemShaders/Particle.hlsl"

cbuffer cbPreSortConstants : register(b0)
{
	uint fillEndIndex;
	uint fillNumber;
}

RWStructuredBuffer<Particle> particles		: register(u0);

RWStructuredBuffer<uint> aliveIndices	: register(u1);
RWByteAddressBuffer counters			: register(u2);


[numthreads(256, 1, 1)]
void PreSortCS(
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	uint id = numAlives + dispatchThreadId.x;

	if (id < fillEndIndex)
	{
		aliveIndices[id] = fillNumber;
	}

	if (dispatchThreadId.x == 0)
	{
		Particle newParticle;
		newParticle.Position = float3(0.0f, 0.0f, 0.0f);
		newParticle.InitialSize = 1.0f;
		newParticle.Velocity = float3(0.0f, 0.0f, 0.0f);
		newParticle.InitialLifetime = 1.0f;
		newParticle.Acceleration = float3(0.0f, 0.0f, 0.0f);
		newParticle.InitialOpacity = 1.0f;
		newParticle.InitialColor = float3(1.0f, 1.0f, 1.0f);
		newParticle.RemainLifetime = 0.0f;
		newParticle.EndColor = float3(1.0f, 1.0f, 1.0f);
		newParticle.EndSize = 0.01f;
		newParticle.EndOpacity = 0.0f;
		newParticle.SpawnTime = 10000.0f;
		newParticle.SpawnOrderInFrame = 0;

		particles[fillNumber] = newParticle;
	}
}