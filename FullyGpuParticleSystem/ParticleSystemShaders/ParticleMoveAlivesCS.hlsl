#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleBuffers.hlsl"

#define NUM_THREADS 256

cbuffer cbUpdateConstants : register(b0)
{
	uint NumParticlesMayBeExpired;
	float DeltaTime;
	float TotalTime;
}

cbuffer cbCounters : register(b1)
{
	uint NumAlivesNext;
	uint NumAlivesBeforeFrame;
	uint NumSurvived;
}

[numthreads(NUM_THREADS, 1, 1)]
void MoveAlivesCS(
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint numAlivesBeforeFrame = NumAlivesBeforeFrame;

	// the number of survived particles in destroyCS phase before.
	uint numSurvived = NumSurvived;

	// the number of destroyed particles in destroyCS phase before.
	uint numDestroyed =
		min(numAlivesBeforeFrame, NumParticlesMayBeExpired) - numSurvived;

	const uint id = NumParticlesMayBeExpired + dispatchThreadId.x;
	uint newNumAlives = numAlivesBeforeFrame - numDestroyed;

	if (id < numAlivesBeforeFrame)
	{
		uint newIndex = numSurvived + dispatchThreadId.x;

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