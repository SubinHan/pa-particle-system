uint hash(uint x)
{
	x += (x << 10u);
	x ^= (x >> 6u);
	x += (x << 3u);
	x ^= (x >> 11u);
	x += (x << 15u);

	return x;
}

float floatConstruct(uint m)
{
	const uint ieeeMantissa = 0x007FFFFFu;
	const uint ieeeOne = 0x3F800000u;

	m &= ieeeMantissa;
	m |= ieeeOne;

	float f = asfloat(m); // Range [1:2]
	return f - 1.0f;
}


// Pseudo-random value in half-open range [0:1].
// by https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float random(float x)
{
	return floatConstruct(hash(asuint(x)));
}


float2 subUv(float2 texC, uint subUvDimensionX, uint subUvDimensionY, uint indexX, uint indexY)
{
	float subUvDimensionXInv = 1.0f / subUvDimensionX;
	float subUvDimensionYInv = 1.0f / subUvDimensionY;

	float2 subUvSize = float2(subUvDimensionXInv, subUvDimensionYInv);
	float2 actualTexC = texC * float2(subUvDimensionXInv, subUvDimensionYInv);

	return float2(subUvSize.x * indexX, subUvSize.y * indexY) + actualTexC;
}

float3 catmullRom(float3 p0, float3 p1, float3 p2, float3 p3, float t)
{
	float3 result;

	const float t2 = t * t;
	const float t3 = t2 * t;
	
	result =
		0.5f * (
			(2.0f * p1) + (-p0 + p2) * t +
			(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
			(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);

	return result;
}

float3 dCatmullRom(float3 p0, float3 p1, float p2, float3 p3, float t)
{
	const float t2 = t * t;

	return 0.5f * (
		(-p1 + p2) +
		(4.0f * p0 - 10.0f * p1 + 8.0f * p2 - 2.0f * p3) * t +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t2);
}

uint packUnorm4ToUint(float4 unpackedData)
{
	uint result = 0;

	result += uint(unpackedData.x * 255.0f);
	result <<= 8;
	result += uint(unpackedData.y * 255.0f);
	result <<= 8;
	result += uint(unpackedData.z * 255.0f);
	result <<= 8;
	result += uint(unpackedData.w * 255.0f);

	return result;
}

float4 unpackUintToUnorm4(uint packedData)
{
	float4 result;

	result.x = float( packedData				>> 24)	 * 0.00392156f;
	result.y = float((packedData & 0x00FF0000)	>> 16)	 * 0.00392156f;
	result.z = float((packedData & 0x0000FF00)	>> 8 )	 * 0.00392156f;
	result.w = float( packedData & 0x000000FF        )	 * 0.00392156f;

	return result;
}