#include "ParticleSystemShaders/Util.hlsl"
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
		particlesCurrent[id].Position = float3(0.0f, 0.0f, 0.0f);
		particlesCurrent[id].InitialSize = 1.0f;
		particlesCurrent[id].Velocity = float3(0.0f, 0.0f, 0.0f);
		particlesCurrent[id].InitialLifetime = 1.0f;
		particlesCurrent[id].Acceleration = float3(0.0f, 0.0f, 0.0f);
		particlesCurrent[id].InitialColor = packUnorm4ToUint(float4(1.0f, 1.0f, 1.0f, 1.0f));
		particlesCurrent[id].RemainLifetime = 1.0f;
		particlesCurrent[id].EndColor = packUnorm4ToUint(float4(1.0f, 1.0f, 1.0f, 1.0f));
		particlesCurrent[id].EndSize = 0.01f;
		particlesCurrent[id].SpawnTime = 10000.0f;
		particlesCurrent[id].DistanceFromPrevious = 0.0f;
		particlesCurrent[id].DistanceFromStart = 0.0f;
	}
}