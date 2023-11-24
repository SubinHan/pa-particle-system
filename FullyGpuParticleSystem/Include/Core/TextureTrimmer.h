#pragma once

#include "Util/MathHelper.h"

#include <string>
#include <vector>
#include <unordered_map>

class TextureTrimmer
{
public:
	using Point = DirectX::XMFLOAT2;

	TextureTrimmer(std::string pngPath);

	// return bounding k-gon.
	std::vector<Point> getBoundingPolygon(int k);
	constexpr int getMaxK();

private:
	void initTextureOutlinePoints(std::string pngPath);
	void initConvexHullVertices(const std::vector<Point>& outlinePoints);
	// make bounding k-gon
	void initBoundingPolygon(const std::vector<Point>& convexHullVertices, int k);

	bool willBeDiscarded(UINT r, UINT g, UINT b, UINT a);

private:
	std::vector<Point> _textureOutlinePoints;
	std::vector<Point> _convexHullVertices;
	std::unordered_map<int, std::vector<Point>> _boundingPolygons;
};

struct Simplifier
{
	static constexpr float BIG_FLOAT = 1e12f;
	static constexpr float EPS_FLOAT = 1e-6f;

	using float2 = DirectX::XMFLOAT2;

	std::vector<float2> vertices;
	std::vector<std::vector<float> > a_cut;
	std::vector<std::vector<float> > a_complement;

	std::vector<std::vector<int> > m_balanced;
	std::vector<std::vector<float> > a_balanced;
	std::vector<std::vector<std::vector<int> > > m_flushed;
	std::vector<std::vector<std::vector<float> > > a_flushed;

	Simplifier(std::vector<float2> v, int k);

	int wrap(int i);

	void calc_balanced(int i, int j, int l, int r);
	void calc_flushed(int h, int i, int j, int l, int r);
	float extra_area_balanced(int i, int j, int k, /*out*/ float2& p, /*out*/ float2& q);
	void recurse_balanced(int i, int j1, int j2);
	void recurse_flushed(int h, int i, int j1, int j2);
	void reconstruct_flushed(int h, int i, int j, std::vector<int>& out_edges);
	void initialize();
	void run();

}; 