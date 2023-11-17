#include "ParticleSystemShaders/Particle.hlsl"

RWStructuredBuffer<Particle> particlesCurrent	: register(u0);
RWStructuredBuffer<Particle> particlesNext		: register(u1);

globallycoherent RWByteAddressBuffer counters	: register(u2);
