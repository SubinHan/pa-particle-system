#include "Util.hlsl"
#include "Particle.hlsl"

// TODO: complete particle struct, cb, emitter struct

cbuffer cbEmitConstants : register(b0)
{
	uint EmitCount;
	float3 Postion;

	float2 Orientation;
	float DeltaTime;
}

RWStructuredBuffer<Particle> particles	: register(u0);

RWStructuredBuffer<uint> aliveIndices	: register(u1);
RWStructuredBuffer<uint> deadIndices	: register(u2);

RWByteAddressBuffer counters			: register(u3);

#define NUM_THREADS 256

// TODO: should not emit when num of particles > MAX_NUM_PARTICLES
// each thread emits a particle.
[numthreads(256, 1, 1)]
void EmitCS(
	int3 groupThreadId : SV_GroupThreadID,
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	if (uint(dispatchThreadId.x) >= EmitCount)
	{
		return;
	}

	// TODO: init Particle with Emit Constants
	Particle newParticle;

	float particleVelocityX = random(float(dispatchThreadId.x) + DeltaTime + 0.1f);
	float particleVelocityY = random(float(dispatchThreadId.x) + DeltaTime + 0.2f);
	float particleVelocityZ = random(float(dispatchThreadId.x) + DeltaTime + 0.3f);

	newParticle.Position =
		float3(0.0f, 0.0f, 0.0f);

	newParticle.Velocity =
		float3(
			particleVelocityX * 0.3f,
			particleVelocityY * 0.3f,
			particleVelocityZ * 0.3f);

	newParticle.Acceleration =
		float3(0.0f, 0.0f, 0.0f);

	newParticle.Lifetime = 2.0f;
	newParticle.Size = 0.01f;
	newParticle.Opacity = 1.0f;
	newParticle.Color = float3(1.0f, 0.0f, 0.0f);

	// add particle into buffer
	// TODO: remove either numDeads or numAlives and derive it with max num of particles.
	uint numDeads;
	counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMDEADS, -1, numDeads);

	uint newParticleIndex = deadIndices[numDeads - 1];

	uint numAlives;
	counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES, 1, numAlives);
	
	aliveIndices[numAlives] = newParticleIndex;

	particles[newParticleIndex] = newParticle;
}