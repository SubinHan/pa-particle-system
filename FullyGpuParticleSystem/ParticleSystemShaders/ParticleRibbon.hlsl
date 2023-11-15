#include "ParticleSystemShaders/Particle.hlsl"
#include "ParticleSystemShaders/Util.hlsl"
#include "ParticleSystemShaders/ParticleRenderPass.hlsl"

RWStructuredBuffer<Particle> particles : register(u0);
RWStructuredBuffer<uint> aliveIndices : register(u1);
RWByteAddressBuffer counters			: register(u2);

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

	float4 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
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

	float4 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
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

	float4 upView = float4(0.0f, 1.0f, 0.0f, 0.0f);
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
	dout.PosW = posW.xyz;
	dout.NormalW = float3(0.0f, 0.0f, 0.0f); // unused

	float2 upTexU = lerp(ribbonPatch[0].TexC, ribbonPatch[2].TexC, uv.x);
	float2 downTexU = lerp(ribbonPatch[1].TexC, ribbonPatch[3].TexC, uv.x);
	float2 texC = lerp(upTexU, downTexU, uv.y);
	dout.TexC = texC;
	dout.LocalTexC = uv;
	dout.ThreadId = ribbonPatch[0].ThreadId;

	return dout;
}