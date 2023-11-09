#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/ParticleSystem.hlsl"
#include "ParticleSystemShaders/Util.hlsl"

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

struct SpriteVertexOut
{
	float3 CenterW	: POSITION;
	float Size : SIZE;
	uint ThreadId : THREADID;
};

struct SpritePixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	uint ThreadId : THREADID;
};

StructuredBuffer<Particle> particles : register(t0);
StructuredBuffer<uint> aliveIndices : register(t1);

%t

RWByteAddressBuffer counters			: register(u0);

%u

SamplerState gsamPointWrap  : register(s0);
SamplerState gsamPointClamp  : register(s1);
SamplerState gsamLinearWrap  : register(s2);
SamplerState gsamLinearClamp  : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp  : register(s5);

SpriteVertexOut SpriteParticleVS(
	uint vid : SV_VertexID)
{
	const uint threadId = vid;

	const uint particleIndex = aliveIndices[threadId];
	Particle particle = particles[particleIndex];

	SpriteVertexOut vertexOut;

	const float4 posW = mul(float4(particle.Position, 1.0f), gWorld);
	vertexOut.CenterW = posW;

	float initialLifetime = particle.InitialLifetime;
	float remainLifetime = particle.RemainLifetime;
	float normalizedLifetimeInv = (initialLifetime - remainLifetime) / initialLifetime;

	float initialSize = particle.InitialSize;
	float endSize = particle.EndSize;
	float interpolatedSize = lerp(initialSize, endSize, normalizedLifetimeInv);

	vertexOut.Size = interpolatedSize;
	vertexOut.ThreadId = threadId;

	return vertexOut;
}

[maxvertexcount(4)]
void SpriteParticleGS(
	point SpriteVertexOut gin[1],
	inout TriangleStream<SpritePixelIn> triStream)
{
	float3 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
	float3 up = mul(upView, gInvView).xyz;
	float3 look = normalize(gEyePosW - gin[0].CenterW);
	
	float3 u = cross(look, up);
	float3 v = cross(look, u);

	float halfWidth = 0.5f * gin[0].Size;
	float halfHeight = 0.5f * gin[0].Size;

	float4 vertices[4];
	vertices[0] = float4(gin[0].CenterW + halfWidth * u - halfHeight * v, 1.0f);
	vertices[1] = float4(gin[0].CenterW + halfWidth * u + halfHeight * v, 1.0f);
	vertices[2] = float4(gin[0].CenterW - halfWidth * u - halfHeight * v, 1.0f);
	vertices[3] = float4(gin[0].CenterW - halfWidth * u + halfHeight * v, 1.0f);

	float2 texC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	SpritePixelIn geoOut;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		geoOut.PosH = mul(vertices[i], gViewProj);
		geoOut.PosW = vertices[i].xyz;
		geoOut.NormalW = look;
		geoOut.TexC = texC[i];
		geoOut.ThreadId = gin[0].ThreadId;

		triStream.Append(geoOut);
	}
}

float4 ParticlePS(SpritePixelIn pin) : SV_Target
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	const uint particleIndex = aliveIndices[pin.ThreadId];
	Particle particle = particles[particleIndex];

	float initialLifetime = particle.InitialLifetime;
	float remainLifetime = particle.RemainLifetime;
	float normalizedLifetimeInv = (initialLifetime - remainLifetime) / initialLifetime;

	float3 initialColor = particle.InitialColor;
	float3 endColor = particle.EndColor;
	float3 interpolatedColor = lerp(initialColor, endColor, normalizedLifetimeInv);

	float initialOpacity = particle.InitialOpacity;
	float endOpacity = particle.EndOpacity;
	float interpolatedOpacity = lerp(initialOpacity, endOpacity, normalizedLifetimeInv);

	color = float4(interpolatedColor, interpolatedOpacity);

	%s

	return color;
}

struct RibbonVertexOut
{
	float3 PosL : POSITION;
	float Size : SIZE;
	float DistanceFromStart : DISTANCE;
	uint ThreadId : THREADID;
};

RibbonVertexOut RibbonParticleVS(
	uint vid : SV_VertexID)
{
	RibbonVertexOut vertexOut;

	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	const uint threadId = min(vid, numAlives - 1);
	const uint particleIndex = aliveIndices[threadId];
	Particle particle = particles[particleIndex];

	vertexOut.PosL = particle.Position;
	vertexOut.ThreadId = threadId;
	vertexOut.DistanceFromStart = particle.DistanceFromStart;

	float initialLifetime = particle.InitialLifetime;
	float remainLifetime = particle.RemainLifetime;
	float normalizedLifetimeInv = (initialLifetime - remainLifetime) / initialLifetime;

	float initialSize = particle.InitialSize;
	float endSize = particle.EndSize;
	float interpolatedSize = lerp(initialSize, endSize, normalizedLifetimeInv);
	vertexOut.Size = interpolatedSize;

	return vertexOut;
}

struct PatchTess
{
	float EdgeTess[4]   : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

PatchTess ConstantHS(InputPatch<RibbonVertexOut, 4> patch, uint PatchID : SV_PrimitiveID)
{
	PatchTess pt;

	float tess = 10.0f;

	pt.EdgeTess[0] = tess;
	pt.EdgeTess[1] = tess;
	pt.EdgeTess[2] = tess;
	pt.EdgeTess[3] = tess;

	pt.InsideTess[0] = tess;
	pt.InsideTess[1] = tess;

	return pt;
}

struct RibbonHullOut
{
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
	uint ThreadId : THREADID;
	float3 ControlPoint0 : CONTROLPOINT0;
	float3 ControlPoint1 : CONTROLPOINT1;
	float3 ControlPoint2 : CONTROLPOINT2;
	float3 ControlPoint3 : CONTROLPOINT3;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
RibbonHullOut RibbonParticleHS_SegmentBased(InputPatch<RibbonVertexOut, 4> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	RibbonHullOut hout;

	float3 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
	float3 up = mul(upView, gInvView).xyz;

	const float3 offset0 = up * p[0].Size;
	const float3 offset1 = up * p[1].Size;
	const float3 offset2 = up * p[2].Size;
	const float3 offset3 = up * p[3].Size;

	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	if (i == 0)
	{
		hout.PosL = p[1].PosL + offset1;
		hout.TexC = float2(0.0f, 0.0f);
		hout.ControlPoint0 = p[0].PosL + offset0;
		hout.ControlPoint1 = p[1].PosL + offset1;
		hout.ControlPoint2 = p[2].PosL + offset2;
		hout.ControlPoint3 = p[3].PosL + offset3;
		hout.ThreadId = p[1].ThreadId;
	}
	else if (i == 1)
	{
		hout.PosL = p[1].PosL - offset1;
		hout.TexC = float2(0.0f, 1.0f);
		hout.ControlPoint0 = p[0].PosL - offset0;
		hout.ControlPoint1 = p[1].PosL - offset1;
		hout.ControlPoint2 = p[2].PosL - offset2;
		hout.ControlPoint3 = p[3].PosL - offset3;
		hout.ThreadId = p[1].ThreadId;
	}
	else if (i == 2)
	{
		hout.PosL = p[2].PosL + offset2;
		hout.TexC = float2(1.0f, 0.0f);
		hout.ControlPoint0 = p[0].PosL + offset0;
		hout.ControlPoint1 = p[1].PosL + offset1;
		hout.ControlPoint2 = p[2].PosL + offset2;
		hout.ControlPoint3 = p[3].PosL + offset3;
		hout.ThreadId = p[2].ThreadId;
	}
	else
	{
		hout.PosL = p[2].PosL - offset2;
		hout.TexC = float2(1.0f, 1.0f);
		hout.ControlPoint0 = p[0].PosL - offset0;
		hout.ControlPoint1 = p[1].PosL - offset1;
		hout.ControlPoint2 = p[2].PosL - offset2;
		hout.ControlPoint3 = p[3].PosL - offset3;
		hout.ThreadId = p[2].ThreadId;
	}

	return hout;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
RibbonHullOut RibbonParticleHS_Stretched(InputPatch<RibbonVertexOut, 4> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	RibbonHullOut hout;

	float3 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
	float3 up = mul(upView, gInvView).xyz;

	const float3 offset0 = up * p[0].Size;
	const float3 offset1 = up * p[1].Size;
	const float3 offset2 = up * p[2].Size;
	const float3 offset3 = up * p[3].Size;

	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	float texU0 = float(p[1].ThreadId) / float(numAlives);
	float texU1 = float(p[2].ThreadId) / float(numAlives);

	if (i == 0)
	{
		hout.PosL = p[1].PosL + offset1;
		hout.TexC = float2(texU0, 1.0f);
		hout.ControlPoint0 = p[0].PosL + offset0;
		hout.ControlPoint1 = p[1].PosL + offset1;
		hout.ControlPoint2 = p[2].PosL + offset2;
		hout.ControlPoint3 = p[3].PosL + offset3;
		hout.ThreadId = p[1].ThreadId;
	}
	else if (i == 1)
	{
		hout.PosL = p[1].PosL - offset1;
		hout.TexC = float2(texU0, 0.0f);
		hout.ControlPoint0 = p[0].PosL - offset0;
		hout.ControlPoint1 = p[1].PosL - offset1;
		hout.ControlPoint2 = p[2].PosL - offset2;
		hout.ControlPoint3 = p[3].PosL - offset3;
		hout.ThreadId = p[1].ThreadId;
	}
	else if (i == 2)
	{
		hout.PosL = p[2].PosL + offset2;
		hout.TexC = float2(texU1, 1.0f);
		hout.ControlPoint0 = p[0].PosL + offset0;
		hout.ControlPoint1 = p[1].PosL + offset1;
		hout.ControlPoint2 = p[2].PosL + offset2;
		hout.ControlPoint3 = p[3].PosL + offset3;
		hout.ThreadId = p[2].ThreadId;
	}
	else
	{
		hout.PosL = p[2].PosL - offset2;
		hout.TexC = float2(texU1, 0.0f);
		hout.ControlPoint0 = p[0].PosL - offset0;
		hout.ControlPoint1 = p[1].PosL - offset1;
		hout.ControlPoint2 = p[2].PosL - offset2;
		hout.ControlPoint3 = p[3].PosL - offset3;
		hout.ThreadId = p[2].ThreadId;
	}

	return hout;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
RibbonHullOut RibbonParticleHS_DistanceBased(InputPatch<RibbonVertexOut, 4> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	RibbonHullOut hout;

	float3 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
	float3 up = mul(upView, gInvView).xyz;

	const float3 offset0 = up * p[0].Size;
	const float3 offset1 = up * p[1].Size;
	const float3 offset2 = up * p[2].Size;
	const float3 offset3 = up * p[3].Size;

	uint numAlives = counters.Load(PARTICLECOUNTER_OFFSET_NUMALIVES);

	float texU0 = p[1].DistanceFromStart * 1.0f;
	float texU1 = p[2].DistanceFromStart * 1.0f;

	if (i == 0)
	{
		hout.PosL = p[1].PosL + offset1;
		hout.TexC = float2(texU0, 1.0f);
		hout.ControlPoint0 = p[0].PosL + offset0;
		hout.ControlPoint1 = p[1].PosL + offset1;
		hout.ControlPoint2 = p[2].PosL + offset2;
		hout.ControlPoint3 = p[3].PosL + offset3;
		hout.ThreadId = p[1].ThreadId;
	}
	else if (i == 1)
	{
		hout.PosL = p[1].PosL - offset1;
		hout.TexC = float2(texU0, 0.0f);
		hout.ControlPoint0 = p[0].PosL - offset0;
		hout.ControlPoint1 = p[1].PosL - offset1;
		hout.ControlPoint2 = p[2].PosL - offset2;
		hout.ControlPoint3 = p[3].PosL - offset3;
		hout.ThreadId = p[1].ThreadId;
	}
	else if (i == 2)
	{
		hout.PosL = p[2].PosL + offset2;
		hout.TexC = float2(texU1, 1.0f);
		hout.ControlPoint0 = p[0].PosL + offset0;
		hout.ControlPoint1 = p[1].PosL + offset1;
		hout.ControlPoint2 = p[2].PosL + offset2;
		hout.ControlPoint3 = p[3].PosL + offset3;
		hout.ThreadId = p[2].ThreadId;
	}
	else
	{
		hout.PosL = p[2].PosL - offset2;
		hout.TexC = float2(texU1, 0.0f);
		hout.ControlPoint0 = p[0].PosL - offset0;
		hout.ControlPoint1 = p[1].PosL - offset1;
		hout.ControlPoint2 = p[2].PosL - offset2;
		hout.ControlPoint3 = p[3].PosL - offset3;
		hout.ThreadId = p[2].ThreadId;
	}

	return hout;
}

struct RibbonPixelIn
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC : TEXCOORD;
	float2 LocalTexC : LOCALTEXCOORD;
	uint ThreadId : THREADID;
};

[domain("quad")]
RibbonPixelIn RibbonParticleDS(PatchTess patchTess,
	float2 uv : SV_DomainLocation,
	const OutputPatch<RibbonHullOut, 4> ribbonPatch)
{
	RibbonPixelIn dout;

	float t = uv.x;

	float3 upPosU = catmullRom(
		ribbonPatch[0].ControlPoint0,
		ribbonPatch[0].ControlPoint1,
		ribbonPatch[0].ControlPoint2,
		ribbonPatch[0].ControlPoint3,
		t);

	float3 downPosU = catmullRom(
		ribbonPatch[1].ControlPoint0,
		ribbonPatch[1].ControlPoint1,
		ribbonPatch[1].ControlPoint2,
		ribbonPatch[1].ControlPoint3,
		t);

	float3 pos = lerp(upPosU, downPosU, uv.y);

	float4 posW = mul(float4(pos, 1.0f), gWorld);
	dout.PosH = mul(posW, gViewProj);
	dout.PosW = posW;
	dout.NormalW = float3(0.0f, 0.0f, 0.0f);

	float2 upTexU = lerp(ribbonPatch[0].TexC, ribbonPatch[2].TexC, uv.x);
	float2 downTexU = lerp(ribbonPatch[1].TexC, ribbonPatch[3].TexC, uv.x);
	float2 texC = lerp(upTexU, downTexU, uv.y);
	dout.TexC = texC;
	dout.LocalTexC = uv;
	dout.ThreadId = ribbonPatch[0].ThreadId;

	return dout;
}

float4 RibbonParticlePS(RibbonPixelIn pin) : SV_Target
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	const uint particleIndex = aliveIndices[pin.ThreadId];
	Particle particle = particles[particleIndex];

	float initialLifetime = particle.InitialLifetime;
	float remainLifetime = particle.RemainLifetime;
	float normalizedLifetimeInv = (initialLifetime - remainLifetime) / initialLifetime;

	float3 initialColor = particle.InitialColor;
	float3 endColor = particle.EndColor;
	float3 interpolatedColor = lerp(initialColor, endColor, normalizedLifetimeInv);

	float initialOpacity = particle.InitialOpacity;
	float endOpacity = particle.EndOpacity;
	float interpolatedOpacity = lerp(initialOpacity, endOpacity, normalizedLifetimeInv);

	// previous particle
	uint previousThreadId = pin.ThreadId - 1;
	if (pin.ThreadId == 0)
	{
		previousThreadId = 0;
	}

	const uint previousParticleIndex = aliveIndices[previousThreadId];
	Particle previousParticle = particles[previousParticleIndex];

	float previousInitialLifetime = previousParticle.InitialLifetime;
	float previousRemainLifetime = previousParticle.RemainLifetime;
	float previousNormalizedLifetimeInv = (previousInitialLifetime - previousRemainLifetime) / previousInitialLifetime;

	float3 previousInitialColor = previousParticle.InitialColor;
	float3 previousEndColor = previousParticle.EndColor;
	float3 previousInterpolatedColor = lerp(previousInitialColor, previousEndColor, previousNormalizedLifetimeInv);

	float previousInitialOpacity = previousParticle.InitialOpacity;
	float previousEndOpacity = previousParticle.EndOpacity;
	float previousInterpolatedOpacity = lerp(previousInitialOpacity, previousEndOpacity, previousNormalizedLifetimeInv);

	interpolatedColor = lerp(previousInterpolatedColor, interpolatedColor, pin.LocalTexC.x);
	interpolatedOpacity = lerp(previousInterpolatedOpacity, interpolatedOpacity, pin.LocalTexC.x);

	color = float4(interpolatedColor, interpolatedOpacity);

	%s

	return color;
}
