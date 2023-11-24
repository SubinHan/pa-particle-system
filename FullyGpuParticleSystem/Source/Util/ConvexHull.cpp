#include "Util/ConvexHull.h"

#include <algorithm>

using Point = DirectX::XMFLOAT2;
using Vector = DirectX::XMFLOAT2;

DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);

DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);

DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);

DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);

DirectX::XMFLOAT2 operator*(const float& lhs, const DirectX::XMFLOAT2& rhs);

DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, const float& rhs);

float dot(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);

float cross(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs);
static DirectX::XMFLOAT2 intersection(const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b, const DirectX::XMFLOAT2& c, const DirectX::XMFLOAT2& d)
{
	static constexpr float BIG_FLOAT = 1e12f;
	static constexpr float EPS_FLOAT = 1e-6f;

	DirectX::XMFLOAT2 ab = b - a, cd = d - c, ac = c - a;
	float num = cross(ab, ac);
	float denom = cross(cd, ab);
	if (denom == 0) return DirectX::XMFLOAT2(-BIG_FLOAT, -BIG_FLOAT);
	float t = num / denom;
	float s = 0;
	if (std::abs(ab.x) > std::abs(ab.y)) s = (ac.x + cd.x * t) / ab.x;
	else s = (ac.y + cd.y * t) / ab.y;
	return DirectX::XMFLOAT2(s, t);
}

ConvexHull::ConvexHull(std::vector<Point> points) :
	_points(points),
	_convexHullVertices()
{
	grahamScan();
}

std::vector<ConvexHull::Point> ConvexHull::getConvexHullVertices()
{
	return _convexHullVertices;
}

float ccw(Point p1, Point p2, Point p3)
{
	return (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
}

void ConvexHull::grahamScan()
{
	// find lowest point and swap with first element.
	float minIndex = 0;
	for (int i = 0; i < _points.size(); ++i)
	{
		if (_points[i].y < _points[minIndex].y ||
			(_points[i].y == _points[minIndex].y && _points[i].x < _points[minIndex].x))
		{
			minIndex = i;
		}
	}
	std::swap(_points[0], _points[minIndex]);

	// sort
	std::sort(_points.data() + 1, _points.data() + _points.size(), [this](Point p1, Point p2) -> bool
		{
			Vector v1 = p1 - _points[0];
			Vector v2 = p2 - _points[0];

			float crossResult = cross(v1, v2);

			if (crossResult != 0)
				return crossResult > 0;

			if (p1.y != p2.y)
				return p1.y < p2.y;

			return p1.x < p2.x;
		});

	// graham scan
	int next = 0;
	Point top;
	Point nextToTop;

	while (next < _points.size())
	{
		while (_convexHullVertices.size() > 1)
		{
			top = _convexHullVertices[_convexHullVertices.size() - 1];
			nextToTop = _convexHullVertices[_convexHullVertices.size() - 2];
			
			if (ccw(nextToTop, top, _points[next]) > 0)
			{
				break;
			}

			_convexHullVertices.pop_back();
		}
		_convexHullVertices.push_back(_points[next++]);
	}
}

bool ConvexHull::compare(Point p1, Point p2)
{
	Vector v1 = p1 - _points[0];
	Vector v2 = p2 - _points[0];

	float crossResult = cross(v1, v2);

	if (crossResult != 0)
		return crossResult > 0;

	if (p1.y != p2.y)
		return p1.y < p2.y;

	return p1.x < p2.x;
}

float ConvexHull::cross(Vector v1, Vector v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}
