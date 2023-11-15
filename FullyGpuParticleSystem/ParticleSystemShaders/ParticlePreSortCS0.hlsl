#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbPreSortConstants : register(b0)
{
	uint fillEndIndex;
	uint fillNumber;
}

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
		newParticle.DistanceFromPrevious = 0.0f;
		newParticle.DistanceFromStart = 0.0f;

		particles[0] = newParticle;
	}
}