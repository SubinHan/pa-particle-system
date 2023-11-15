#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/ParticleSimulateUtil.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbUpdateConstants : register(b0)
{
	float DeltaTime;
	float TotalTime;
}

%t

%u

SamplerState gsamPointWrap  : register(s0);
SamplerState gsamPointClamp  : register(s1);
SamplerState gsamLinearWrap  : register(s2);
SamplerState gsamLinearClamp  : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp  : register(s5);

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
		particles[particleIndex].RemainLifetime -= DeltaTime;

		bool isExpired = particles[particleIndex].RemainLifetime <= 0.f;

		if (isExpired)
		{
			// kill
			uint numDeads;
			counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMDEADS, 1, numDeads);

			deadIndices[numDeads] = particleIndex;
			
			particles[particleIndex] = (Particle)0;
		}
		else
		{
			float3 currentVelocity = particles[particleIndex].Velocity;
			float3 currentPosition = particles[particleIndex].Position;
			float3 currentAcceleration = particles[particleIndex].Acceleration;

			%s

			currentVelocity += currentAcceleration * DeltaTime;
			particles[particleIndex].Position +=
				(currentVelocity + particles[particleIndex].Velocity) * DeltaTime * 0.5f;
			particles[particleIndex].Velocity = currentVelocity;

			// move alive particles to ping-pong buffer 
			uint newIndex;
			counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES_POST_UPDATE, 1, newIndex);

			newAliveIndices[newIndex] = particleIndex;
		}
	}
}