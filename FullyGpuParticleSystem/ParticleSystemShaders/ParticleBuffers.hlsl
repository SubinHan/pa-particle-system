#include "ParticleSystemShaders/Particle.hlsl"

RWStructuredBuffer<Particle> particles		: register(u0);

RWStructuredBuffer<uint> aliveIndices		: register(u1);
RWStructuredBuffer<uint> newAliveIndices	: register(u2);
RWStructuredBuffer<uint> deadIndices		: register(u3);

RWByteAddressBuffer counters				: register(u4);
