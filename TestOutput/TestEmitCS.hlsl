#include "Util.hlsl"
#include "ParticleApp/Shaders/Particle.hlsl"
#include "ParticleSystem.hlsl"

cbuffer cbEmitConstants : register(b1)
{
	uint EmitCount;
	float3 Postion;

	float2 Orientation;
	float DeltaTime;
	uint MaxNumParticles;
}

RWStructuredBuffer<Particle> particles	: register(u0);

RWStructuredBuffer<uint> aliveIndices	: register(u1);
RWStructuredBuffer<uint> deadIndices	: register(u2);

RWByteAddressBuffer counters			: register(u3);

#define NUM_THREADS 256

// each thread emits a particle.
[numthreads(256, 1, 1)]
void EmitCS(
	int3 groupThreadId : SV_GroupThreadID,
	int3 dispatchThreadId : SV_DispatchThreadID)
{
	// should not emit if num of particles reached max.
	uint numAlivesBeforeEmit = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);
	DeviceMemoryBarrierWithGroupSync();

	bool willOver = numAlivesBeforeEmit + EmitCount > MaxNumParticles;
	if (willOver)
	{
		return;
	}

	// each thread emits a single particle.
	if (uint(dispatchThreadId.x) >= EmitCount)
	{
		return;
	}

	Particle newParticle;

float3 local0 = float3(0.000000, 0.000000, 0.000000);
float3 local1 = float3(random(0.101251 + DeltaTime), random(0.201251 + DeltaTime), random(0.301251 + DeltaTime));
float3 local2 = float3(0.000000, -0.100000, 0.000000);
float local3 = float(4.000000);
float local4 = float(0.050000);
float local5 = float(1.000000);
newParticle.Position = local0;
newParticle.Velocity = local1;
newParticle.Acceleration = local2;
newParticle.Lifetime = local3;
newParticle.Size = local4;
newParticle.Opacity = local5;

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
