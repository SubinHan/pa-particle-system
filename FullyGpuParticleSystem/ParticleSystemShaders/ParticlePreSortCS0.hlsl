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
		float4 initialColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		float4 endColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		float3 position = float3(0.0f, 0.0f, 0.0f);
		float3 velocity = float3(0.0f, 0.0f, 0.0f);
		float3 acceleration = float3(0.0f, 0.0f, 0.0f);
		float initialLifetime = 1.0f;
		float remainLifetime = 1.0f;
		float initialSize = 0.1f;
		float endSize = 0.1f;
		remainLifetime = initialLifetime;

		Particle newParticle;
		particlesCurrent[id].PositionXY = packFloat2ToUint(position.x, position.y);
		particlesCurrent[id].PositionZVelocityX = packFloat2ToUint(position.z, velocity.x);
		particlesCurrent[id].VelocityYZ = packFloat2ToUint(velocity.y, velocity.z);
		particlesCurrent[id].InitialLifetimeAndRemainLifetime = packFloat2ToUint(initialLifetime, remainLifetime);
		particlesCurrent[id].AccelerationXY = packFloat2ToUint(acceleration.x, acceleration.y);
		particlesCurrent[id].AccelerationZAndSpawnTime = packFloat2ToUint(acceleration.z, 10000.0f);
		particlesCurrent[id].InitialSizeAndEndSize = packFloat2ToUint(initialSize, endSize);
		particlesCurrent[id].DistanceFromPreviousAndDistanceFromStart = packFloat2ToUint(0.0f, 0.0f);
		particlesCurrent[id].InitialColor = packUnorm4ToUint(initialColor);
	}
}