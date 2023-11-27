#include "Core/TextureTrimmer.h"

#include "Util/ImageReader.h"
#include "Util/ConvexHull.h"

#include <assert.h>

static constexpr int MAX_K = 8;

DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	return DirectX::XMFLOAT2(lhs.x + rhs.x, lhs.y + rhs.y);
}

DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	return DirectX::XMFLOAT2(lhs.x - rhs.x, lhs.y - rhs.y);
}

DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	return DirectX::XMFLOAT2(lhs.x * rhs.x, lhs.y * rhs.y);
}

DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	return DirectX::XMFLOAT2(lhs.x / rhs.x, lhs.y / rhs.y);
}

DirectX::XMFLOAT2 operator*(const float& lhs, const DirectX::XMFLOAT2& rhs)
{
	return DirectX::XMFLOAT2(lhs * rhs.x, lhs * rhs.y);
}

DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& lhs, const float& rhs)
{
	return rhs * lhs;
}

float dot(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

float cross(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

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

TextureTrimmer::TextureTrimmer(std::string pngPath) :
	_textureOutlinePoints(),
	_convexHullVertices(),
	_boundingPolygons()
{
	initTextureOutlinePoints(pngPath);
	initConvexHullVertices(_textureOutlinePoints);
	initBoundingPolygon(_convexHullVertices, 8);
}

void TextureTrimmer::initTextureOutlinePoints(std::string pngPath)
{
	ImageReader reader{ pngPath };

	for (int y = 0; y < reader.getHeight(); y += 2)
	{
		float left = static_cast<float>(reader.getWidth() + 1.0f);
		float right = -1.0f;

		for (int x = 0; x < reader.getWidth(); x += 2)
		{
			Pixel p = reader.getPixelAt(x, y);
			if (willBeDiscarded(p._red, p._green, p._blue, 1.0f))
				continue;

			left = min(left, static_cast<float>(x));
			right = max(right, static_cast<float>(x));
		}

		if (left < static_cast<float>(reader.getWidth()) + 0.5f)
			_textureOutlinePoints.emplace_back(left / reader.getWidth(), static_cast<float>(y) / reader.getWidth());
		if (-0.5f < right)
			_textureOutlinePoints.emplace_back(right / reader.getWidth(), static_cast<float>(y) / reader.getWidth());
	}
}

void TextureTrimmer::initConvexHullVertices(const std::vector<Point>& outlinePoints)
{
	ConvexHull convexHull{ outlinePoints };

	_convexHullVertices = convexHull.getConvexHullVertices();
}

void TextureTrimmer::initBoundingPolygon(const std::vector<Point>& convexHullVertices, int k)
{
	if (k < 4 || convexHullVertices.size() < k)
	{
		_boundingPolygons[k] = _convexHullVertices;
		_boundingPolygons[k].resize(k);
		return;
	}
	Simplifier s(convexHullVertices, k);
	s.run();
	_boundingPolygons[k] = s.vertices;
}

std::vector<TextureTrimmer::Point> TextureTrimmer::getBoundingPolygon(int k)
{
	if (_boundingPolygons.find(k) == _boundingPolygons.end())
	{
		assert(0 && "Cannot generate bounding k-gon because k < 3 or k > maxK. You can get maxK by getMaxK().");
	}

	return _boundingPolygons[k];
}

constexpr int TextureTrimmer::getMaxK()
{
	return MAX_K;
}

bool TextureTrimmer::willBeDiscarded(UINT r, UINT g, UINT b, UINT a)
{
	return r < 5;
}

Simplifier::Simplifier(std::vector<float2> v, int k)
	: vertices(v)
	, a_cut(v.size(), std::vector<float>(v.size(), 0.0f))
	, a_complement(v.size(), std::vector<float>(v.size(), 0.0f))
	, m_balanced(v.size(), std::vector<int>(v.size(), -1))
	, a_balanced(v.size(), std::vector<float>(v.size(), BIG_FLOAT))
	, m_flushed(k - 3, std::vector<std::vector<int> >(v.size(), std::vector<int>(v.size(), -1)))
	, a_flushed(k - 3, std::vector<std::vector<float> >(v.size(), std::vector<float>(v.size(), BIG_FLOAT)))
{
	initialize();
}

int Simplifier::wrap(int i)
{
	int n = static_cast<int>(vertices.size());
	int j = i % n;
	return j < 0 ? j + n : j;
}

void Simplifier::calc_balanced(int i, int j, int l, int r)
{
	float min_extra_area = BIG_FLOAT;
	int choose = -1;
	for (int k = l; k != wrap(r + 1) && k != j; k = wrap(k + 1))
	{
		float2 dummy_1, dummy_2;
		float extra_area = extra_area_balanced(i, j, k, dummy_1, dummy_2);
		if (extra_area < min_extra_area)
		{
			min_extra_area = extra_area;
			choose = k;
		}
	}
	m_balanced[i][j] = choose;
	a_balanced[i][j] = min_extra_area;
}

void Simplifier::calc_flushed(int h, int i, int j, int l, int r)
{
	float min_extra_area = BIG_FLOAT;
	int choose = -1;
	for (int k = l; k != wrap(r + 1) && k != j; k = wrap(k + 1))
	{
		float extra_area = BIG_FLOAT;
		int h1 = (h + 1) / 2 - 1;
		int h2 = h - (h + 1) / 2 - 1;
		if (wrap(k - i) > h1 + 1 && wrap(j - k) > h2 + 1)
		{
			float area1 = h1 < 0 ? a_complement[i][k] : a_flushed[h1][i][k];
			float area2 = h2 < 0 ? a_complement[k][j] : a_flushed[h2][k][j];
			extra_area = area1 + area2;
		}
		if (extra_area < min_extra_area)
		{
			min_extra_area = extra_area;
			choose = k;
		}
	}
	m_flushed[h][i][j] = choose;
	a_flushed[h][i][j] = min_extra_area;
}

float Simplifier::extra_area_balanced(int i, int j, int k, float2& p, float2& q)
{
	float2 a = vertices[i], b = vertices[wrap(i + 1)];
	float2 c = vertices[wrap(j + 1)], d = vertices[wrap(j)];
	float2 e = vertices[k];
	float2 ab = b - a, cd = d - c;
	float2 next = vertices[wrap(k + 1)] - e;
	float2 prev = e - vertices[wrap(k - 1)];
	float2 opt = next;
	if (k != wrap(i + 1) && std::abs(cross(ab, cd)) > EPS_FLOAT)
	{
		// a + 2(e-a), c에서 각각 ab, cd방향 직선의 교점
		float t = cross(ab, 2 * e - a - c) / cross(ab, cd);
		opt = c + cd * t - e;
		// 만약 convex hull을 뚫어버리는 balanced point라면, 그냥 prev or next로 조정
		if (cross(prev, opt) < 0) opt = prev;
		if (cross(next, opt) > 0) opt = next;
	}

	float2 s1 = intersection(a, b, e, e - opt);
	float2 s2 = intersection(c, d, e, e + opt);
	if (s1.x < 1 - EPS_FLOAT || s2.x < 1 - EPS_FLOAT) return BIG_FLOAT;

	p = a + ab * s1.x;
	q = c + cd * s2.x;
	float quad_area = (cross(p - b, q - b) + cross(q - b, d - b)) / 2;
	return quad_area - a_cut[i][j];
}

void Simplifier::recurse_balanced(int i, int j1, int j2)
{
	if (wrap(j2 - j1) < 2) return;
	int m = wrap(j1 + wrap(j2 - j1) / 2);
	int l = m_balanced[i][j1] < 0 ? wrap(i + 1) : m_balanced[i][j1];
	int r = m_balanced[i][j2] < 0 ? wrap(m - 1) : m_balanced[i][j2];
	calc_balanced(i, m, l, r);
	recurse_balanced(i, j1, m);
	recurse_balanced(i, m, j2);
}

void Simplifier::recurse_flushed(int h, int i, int j1, int j2)
{
	if (wrap(j2 - j1) < 2) return;
	int m = wrap(j1 + wrap(j2 - j1) / 2);
	int l = m_flushed[h][i][j1] < 0 ? wrap(i + 1) : m_flushed[h][i][j1];
	int r = m_flushed[h][i][j2] < 0 ? wrap(m - 1) : m_flushed[h][i][j2];
	calc_flushed(h, i, m, l, r);
	recurse_flushed(h, i, j1, m);
	recurse_flushed(h, i, m, j2);
}

void Simplifier::reconstruct_flushed(int h, int i, int j, std::vector<int>& out_edges)
{
	if (h < 0 || wrap(j - i) <= h + 1) return;
	int h1 = (h + 1) / 2 - 1;
	int h2 = h - (h + 1) / 2 - 1;
	int k = m_flushed[h][i][j];
	reconstruct_flushed(h1, i, k, out_edges);
	out_edges.emplace_back(k);
	reconstruct_flushed(h2, k, j, out_edges);
}

void Simplifier::initialize()
{
	// init cut area
	for (int i = 0; i != vertices.size(); ++i)
	{
		for (int j = wrap(i + 3); j != i; j = wrap(j + 1))
		{
			float2 v1 = vertices[wrap(j - 1)] - vertices[wrap(i + 1)];
			float2 v2 = vertices[j] - vertices[wrap(j - 1)];
			float area_augment = cross(v1, v2) / 2;
			a_cut[i][j] = a_cut[i][wrap(j - 1)] + area_augment;
		}
	}

	// init complement area
	for (int i = 0; i != vertices.size(); ++i)
	{
		for (int j = wrap(i + 2); j != i; j = wrap(j + 1))
		{
			float2 a = vertices[i], b = vertices[wrap(i + 1)];
			float2 c = vertices[wrap(j + 1)], d = vertices[j];
			float2 s = intersection(a, b, c, d);
			if (s.x < 1 - EPS_FLOAT || s.y < 1 - EPS_FLOAT) a_complement[i][j] = BIG_FLOAT;
			else
			{
				float2 e = a + (b - a) * s.x;
				float2 v1 = d - e, v2 = b - e;
				a_complement[i][j] = cross(v1, v2) / 2 - a_cut[i][j];
			}
		}
	}

	// compute optimal one-sided chains
	for (int i = 0; i != vertices.size(); ++i)
	{
		int j1 = wrap(i + 2), j2 = wrap(i - 2);
		calc_balanced(i, j1, wrap(i + 1), wrap(j1 - 1));
		calc_balanced(i, j2, wrap(i + 1), wrap(j2 - 1));
		recurse_balanced(i, j1, j2);
	}

	// compute optimal flush chains
	for (int h = 0; h != m_flushed.size(); ++h)
	{
		for (int i = 0; i != vertices.size(); ++i)
		{
			int j1 = wrap(i + 2), j2 = wrap(i - 2);
			calc_flushed(h, i, j1, wrap(i + 1), wrap(j1 - 1));
			calc_flushed(h, i, j2, wrap(i + 1), wrap(j2 - 1));
			recurse_flushed(h, i, j1, j2);
		}
	}
}

void Simplifier::run()
{
	// find the optimal cut pair (s, t)
	float min_extra_area = BIG_FLOAT;
	int s = -1, t = -1;
	int h = (int)a_flushed.size() - 1;
	for (int i = 0; i != vertices.size(); ++i)
	{
		for (int j = 0; j != vertices.size(); ++j)
		{
			float extra_area = a_balanced[i][j] + a_flushed[h][j][i];
			if (extra_area < min_extra_area)
			{
				min_extra_area = extra_area;
				s = i; t = j;
			}
		}
	}

	// reconstruct the optimal k-gon
	std::vector<float2> new_vertices(2);
	extra_area_balanced(s, t, m_balanced[s][t], new_vertices[0], new_vertices[1]);

	std::vector<int> flushed_edges;
	flushed_edges.emplace_back(t);
	reconstruct_flushed(h, t, s, flushed_edges);
	flushed_edges.emplace_back(s);

	for (int i = 0; i < static_cast<int>(flushed_edges.size()) - 1; ++i)
	{
		int e1 = flushed_edges[i];
		int e2 = flushed_edges[i + 1];
		float2 a = vertices[e1], b = vertices[wrap(e1 + 1)];
		float2 c = vertices[e2], d = vertices[wrap(e2 + 1)];
		float2 p = intersection(a, b, c, d);
		new_vertices.emplace_back(a + (b - a) * p.x);
	}

	using std::swap;
	swap(vertices, new_vertices);
}
