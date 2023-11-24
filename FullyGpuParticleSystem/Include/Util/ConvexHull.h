#pragma once

#include "Util/MathHelper.h"

#include <vector>

class ConvexHull
{
	using Point = DirectX::XMFLOAT2;
	using Vector = DirectX::XMFLOAT2;

public:
	ConvexHull() = delete;
	ConvexHull(std::vector<Point> points);
	ConvexHull(const ConvexHull& copy) = delete;
	~ConvexHull() = default;
	ConvexHull& operator=(const ConvexHull& rhs) = delete;
	
	std::vector<Point> getConvexHullVertices();

private:
	void grahamScan();
	bool compare(Point p1, Point p2);
	float cross(Vector v1, Vector v2);

private:
	std::vector<Point> _points;
	std::vector<Point> _convexHullVertices;

};