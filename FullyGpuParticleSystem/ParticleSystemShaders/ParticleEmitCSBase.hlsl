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
	uint numAlivesBeforeEmit = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	bool willOver = numAlivesBeforeEmit + EmitCount >= MaxNumParticles;
	if (willOver || uint(dispatchThreadId.x) >= EmitCount)
	{
		return;
	}
	else
	{
		Particle newParticle;
		newParticle.Position = float3(0.0f, 0.0f, 0.0f);
		newParticle.InitialSize = 1.0f;
		newParticle.Velocity = float3(0.0f, 0.0f, 0.0f);
		newParticle.InitialLifetime = 1.0f;
		newParticle.Acceleration = float3(0.0f, 0.0f, 0.0f);
		newParticle.InitialOpacity = 1.0f;
		newParticle.InitialColor = float3(1.0f, 1.0f, 1.0f);
		newParticle.EndColor = float3(1.0f, 1.0f, 1.0f);
		newParticle.EndSize = 0.01f;
		newParticle.EndOpacity = 0.0f;
		newParticle.SpawnTime = TotalTime;
		newParticle.SpawnOrderInFrame = dispatchThreadId.x;
		newParticle.DistanceFromPrevious = 0.0f;
		newParticle.DistanceFromStart = 0.0f;

		%s

		newParticle.RemainLifetime = newParticle.InitialLifetime;
		// add particle into buffer

		uint numAlives;
		counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES, 1, numAlives);

		particlesCurrent[numAlives] = newParticle;
	}
}