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
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	const uint id = dispatchThreadId.x;

	uint numAlives = 
		counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	if (id < numAlives)
	{
		particlesCurrent[id].RemainLifetime -= DeltaTime;

		bool isAlive = particlesCurrent[id].RemainLifetime > 0.f;

		if (isAlive)
		{
			float3 currentVelocity = particlesCurrent[id].Velocity;
			float3 currentPosition = particlesCurrent[id].Position;
			float3 currentAcceleration = particlesCurrent[id].Acceleration;

			%s
			
			// move alive particles to ping-pong buffer 

			uint newIndex;
			counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMALIVES_POST_UPDATE, 1, newIndex);

			particlesNext[newIndex] = particlesCurrent[id];

			currentVelocity += currentAcceleration * DeltaTime;
			particlesNext[newIndex].Position +=
				(currentVelocity + particlesNext[newIndex].Velocity) * DeltaTime * 0.5f;
			particlesNext[newIndex].Velocity = currentVelocity;
		}
	}
}