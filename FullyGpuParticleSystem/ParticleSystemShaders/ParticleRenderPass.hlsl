#ifndef PARTICLE_RENDER_PASS_HLSL
#define PARTICLE_RENDER_PASS_HLSL

cbuffer cbPerParticleSystem : register (b0)
{
	float4x4 gWorld;
}

cbuffer cbPass : register(b1)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;

	float3 gEyePosW;
	float cbPerObjectPad1;

	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;

	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float DeltaTime;
};

#endif