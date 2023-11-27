#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

cbuffer cbUpdateConstants : register(b0)
{
	uint NumParticlesMayBeExpired;
	float DeltaTime;
	float TotalTime;
}

// each thread updates a particle and kills if expired.
[numthreads(256, 1, 1)]
void DestroyCS(
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);
	const uint id = dispatchThreadId.x;

	if (id < NumParticlesMayBeExpired && id < numAlives)
	{
		Particle current = particlesCurrent[id];

		float initialLifetime;
		float remainLifetime;
		unpackUintToFloat2(
			current.InitialLifetimeAndRemainLifetime,
			initialLifetime, 
			remainLifetime);
		remainLifetime -= DeltaTime;
		current.InitialLifetimeAndRemainLifetime =
			packFloat2ToUint(initialLifetime, remainLifetime);

		bool isAlive = remainLifetime > 0.f;

		if (isAlive)
		{
			uint newIndex;
			counters.InterlockedAdd(PARTICLECOUNTER_OFFSET_NUMSURVIVED, 1, newIndex);

			// simulate
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

			unpackUintToFloat2(current.PositionXY, positionX, positionY);
			unpackUintToFloat2(current.PositionZVelocityX, positionZ, velocityX);
			unpackUintToFloat2(current.VelocityYZ, velocityY, velocityZ);
			unpackUintToFloat2(current.AccelerationXY, accelerationX, accelerationY);
			unpackUintToFloat2(current.AccelerationZAndSpawnTime, accelerationZ, spawnTime);

			float3 currentVelocity = float3(velocityX, velocityY, velocityZ);
			float3 currentPosition = float3(positionX, positionY, positionZ);
			float3 currentAcceleration = float3(accelerationX, accelerationY, accelerationZ);

			%s

			float3 beforeVelocity = currentVelocity;
			currentVelocity += currentAcceleration * DeltaTime;
			currentPosition +=
				(currentVelocity + beforeVelocity) * DeltaTime * 0.5f;

			current.PositionXY =
				packFloat2ToUint(currentPosition.x, currentPosition.y);
			current.PositionZVelocityX =
				packFloat2ToUint(currentPosition.z, currentVelocity.x);
			current.VelocityYZ =
				packFloat2ToUint(currentVelocity.y, currentVelocity.z);
			current.AccelerationXY =
				packFloat2ToUint(currentAcceleration.x, currentAcceleration.y);
			current.AccelerationZAndSpawnTime =
				packFloat2ToUint(currentAcceleration.z, spawnTime);

			particlesNext[newIndex] = current;
		}
	}
}