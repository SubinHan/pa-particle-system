#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>
#include <algorithm>

class MathHelper
{
public:
	// Returns random float in [0, 1).
	static float randF()
	{
		return (float)(::rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float randF(float a, float b)
	{
		return a + randF() * (b - a);
	}

	static int rand(int a, int b)
	{
		return a + ::rand() % ((b - a) + 1);
	}

	template<typename T>
	static T lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	static T clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float angleFromXY(float x, float y);

	static DirectX::XMVECTOR sphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius * sinf(phi) * cosf(theta),
			radius * cosf(phi),
			radius * sinf(phi) * sinf(theta),
			1.0f);
	}

	static DirectX::XMMATRIX inverseTranspose(DirectX::CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		DirectX::XMMATRIX A = M;
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

	static DirectX::XMFLOAT4X4 identity4x4()
	{
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}

	static bool nearlyZero(float a)
	{
		constexpr float EPSILON = 1e-5;

		return std::abs(a) < EPSILON;
	}

	static DirectX::XMVECTOR randUnitVec3();
	static DirectX::XMVECTOR randHemisphereUnitVec3(DirectX::XMVECTOR n);

	static const float Infinity;
	static const float Pi;
};