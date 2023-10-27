

float3 computeAttractionForce(
	float3 particlePosition,
	float3 attractorPosition,
	float attractorRadius,
	float strength)
{
	const float3 directionToPoint = attractorPosition - particlePosition + float3(0, 0, 0.0001f);
	const float radiusSquare = attractorRadius * attractorRadius;
	const float distSquare = max(dot(directionToPoint, directionToPoint), radiusSquare);
	const float attraction = strength / distSquare;
	return attraction * normalize(directionToPoint);
}

float3 computeDragForce(
	float3 velocity,
	float dragCoefficient)
{
	return -dragCoefficient * velocity;
}

float3 computeVortexForce(
	float3 particlePosition,
	float3 vortexCenter,
	float3 vortexAxis,
	float magnitude,
	float tightness)
{
	const float3 centerToPosition = particlePosition - vortexCenter;
	const float distance = length(centerToPosition);
	const float3 vortexDirection = normalize(cross(vortexAxis, centerToPosition));

	const float denominator = pow(distance, tightness) + 0.0001f;
	const float numerator = magnitude;

	float theta = numerator / denominator;

	return vortexDirection * theta;
}