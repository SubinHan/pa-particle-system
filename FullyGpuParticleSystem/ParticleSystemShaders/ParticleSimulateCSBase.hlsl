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
		float positionX;
		float positionY;
		float positionZ;
		float velocityX;
		float velocityY;
		float velocityZ;
		float accelerationX;
		float accelerationY;
		float accelerationZ;
		float spawnTime;

		unpackUintToFloat2(particlesCurrent[id].PositionXY, positionX, positionY);
		unpackUintToFloat2(particlesCurrent[id].PositionZVelocityX, positionZ, velocityX);
		unpackUintToFloat2(particlesCurrent[id].VelocityYZ, velocityY, velocityZ);
		unpackUintToFloat2(particlesCurrent[id].AccelerationXY, accelerationX, accelerationY);
		unpackUintToFloat2(particlesCurrent[id].AccelerationZAndSpawnTime, accelerationZ, spawnTime);

		float3 currentVelocity = float3(velocityX, velocityY, velocityZ);
		float3 currentPosition = float3(positionX, positionY, positionZ);
		float3 currentAcceleration = float3(accelerationX, accelerationY, accelerationZ);

		%s

		float3 beforeVelocity = currentVelocity;
		currentVelocity += currentAcceleration * DeltaTime;
		currentPosition +=
			(currentVelocity + beforeVelocity) * DeltaTime * 0.5f;

		particlesCurrent[id].PositionXY = 
			packFloat2ToUint(currentPosition.x, currentPosition.y);
		particlesCurrent[id].PositionZVelocityX = 
			packFloat2ToUint(currentPosition.z, currentVelocity.x);
		particlesCurrent[id].VelocityYZ =
			packFloat2ToUint(currentVelocity.y, currentVelocity.z);
		particlesCurrent[id].AccelerationXY =
			packFloat2ToUint(currentAcceleration.x, currentAcceleration.y);
		particlesCurrent[id].AccelerationZAndSpawnTime =
			packFloat2ToUint(currentAcceleration.z, spawnTime);
	}
}