#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbEmitConstants : register(b0)
{
	float4x4 gWorld;

	uint EmitCount;
	float3 Postion;

	float2 Orientation;
	float DeltaTime;
	uint MaxNumParticles;

	float TotalTime;
}

#define NUM_THREADS 256

// each thread emits a particle.
[numthreads(256, 1, 1)]
void EmitCS(
	int3 groupThreadId : SV_GroupThreadID,
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	// should not emit if num of particles reached max.
	// here is a concurrency issue because of loading numAlives,
	// but it's not urgent issue because it only occurs just lack of emission.
	uint numAlivesBeforeEmit = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	bool willOver = numAlivesBeforeEmit + EmitCount >= MaxNumParticles;
	if (willOver || uint(dispatchThreadId.x) >= EmitCount)
	{
		return;
	}
	else
	{
		float4 initialColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		float4 endColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		float3 position = float3(0.0f, 0.0f, 0.0f);
		float3 velocity = float3(0.0f, 0.0f, 0.0f);
		float3 acceleration = float3(0.0f, 0.0f, 0.0f);
		float initialLifetime = 1.0f;
		float remainLifetime;
		float initialSize = 0.1f;
		float endSize = 0.1f;

		%s

		remainLifetime = initialLifetime;

		Particle newParticle;
		newParticle.PositionXY = packFloat2ToUint(position.x, position.y);
		newParticle.PositionZVelocityX = packFloat2ToUint(position.z, velocity.x);
		newParticle.VelocityYZ = packFloat2ToUint(velocity.y, velocity.z);
		newParticle.InitialLifetimeAndRemainLifetime = packFloat2ToUint(initialLifetime, remainLifetime);
		newParticle.AccelerationXY = packFloat2ToUint(acceleration.x, acceleration.y);
		newParticle.AccelerationZAndSpawnTime = packFloat2ToUint(acceleration.z, TotalTime);
		newParticle.InitialSizeAndEndSize = packFloat2ToUint(initialSize, endSize);
		newParticle.DistanceFromPreviousAndDistanceFromStart = packFloat2ToUint(0.0f, 0.0f);
		newParticle.InitialColor = packUnorm4ToUint(initialColor);
		newParticle.EndColor = packUnorm4ToUint(endColor);

		// add particle into buffer

		uint numAlives;
		counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES, 1, numAlives);

		particlesCurrent[numAlives] = newParticle;
	}
}