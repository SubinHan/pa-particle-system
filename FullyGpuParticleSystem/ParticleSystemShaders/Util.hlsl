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