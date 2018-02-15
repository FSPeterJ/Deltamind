#include "HexagonTile.h"
#include "DebugRenderer.h"
#include <DirectXMath.h>
#include <algorithm>
#include "HexagonalGridLayout.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))

template<typename T>
const std::vector<HexTile> HexagonTile<T>::directions = {
	HexTile(1, 0, -1), HexTile(1, -1, 0), HexTile(0, -1, 1),
	HexTile(-1, 0, 1), HexTile(-1, 1, 0), HexTile(0, 1, -1)
};

template<typename T>
const std::vector<HexTile> HexagonTile<T>::diagonals = {
	HexTile(2, -1, -1), HexTile(1, -2, 1), HexTile(-1, -1, 2),
	HexTile(-2, 1, 1), HexTile(-1, 2, -1), HexTile(1, 1, -2)
};

template<typename T>
HexagonTile<T>& HexagonTile<T>::operator=(HexagonTile<T> b) {
	swap(b);
	return *this;
}

template<typename T>
HexagonTile<T> HexagonTile<T>::operator+=(const HexagonTile<T>& b) {
	q += b.q; r += b.r; s += b.s;
	return *this;
}

template<typename T>
HexagonTile<T> HexagonTile<T>::operator-=(const HexagonTile<T>& b) {
	q -= b.q; r -= b.r; s -= b.s;
	return *this;
}

template<typename T>
HexagonTile<T> HexagonTile<T>::operator*=(const T& b) {
	q *= b; r *= b; s *= b;
	return *this;
}

template<typename T>
inline const float HexagonTile<T>::Length() const { return ((int) (abs(q) + abs(r) + abs(s)))*0.5f; }

DirectX::XMFLOAT2 HexagonTile<int>::Center(HexagonalGridLayout layout) {
	const Orientation& M = layout.orientation;
	double x = (M.f0 * q + M.f1 * r) * layout.size.x;
	double y = (M.f2 * q + M.f3 * r) * layout.size.y;
	return DirectX::XMFLOAT2(float(x + layout.origin.x), float(y + layout.origin.y));
}

template<typename T>
std::vector<HexTile> HexagonTile<T>::Neighbors() {
	std::vector<HexTile> neighbors = {};
	for(size_t i = 0; i < Hexagon::NUMBER_OF_SIDES; ++i) {
		neighbors.push_back(*this + Direction((NEIGHBOR_DIRECTION) i));
	}

	return neighbors;
}

template<typename T>
void HexagonTile<T>::Draw(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset) {
	DirectX::XMFLOAT2 corners[Hexagon::NUMBER_OF_SIDES];
	HexagonalGridLayout::GetHexCorners(*this, layout, &corners[0]);

	DirectX::XMFLOAT3 line0 = {corners[0].x, corners[0].y, offset};
	DirectX::XMFLOAT3 line1 = {corners[1].x, corners[1].y, offset};
	DirectX::XMFLOAT3 line2 = {corners[2].x, corners[2].y, offset};
	DirectX::XMFLOAT3 line3 = {corners[3].x, corners[3].y, offset};
	DirectX::XMFLOAT3 line4 = {corners[4].x, corners[4].y, offset};
	DirectX::XMFLOAT3 line5 = {corners[5].x, corners[5].y, offset};

	DebugRenderer::AddLine(line0, line1, color);
	DebugRenderer::AddLine(line1, line2, color);
	DebugRenderer::AddLine(line2, line3, color);
	DebugRenderer::AddLine(line3, line4, color);
	DebugRenderer::AddLine(line4, line5, color);
	DebugRenderer::AddLine(line5, line0, color);
}

template<typename T>
void HexagonTile<T>::DrawX(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset) {
	DirectX::XMFLOAT2 corners[Hexagon::NUMBER_OF_SIDES];
	HexagonalGridLayout::GetHexCorners(*this, layout, &corners[0]);

	DirectX::XMFLOAT3 line0 = {corners[1].x, corners[4].y, offset};
	DirectX::XMFLOAT3 line1 = {corners[4].x, corners[1].y, offset};
	DirectX::XMFLOAT3 line2 = {corners[2].x, corners[5].y, offset};
	DirectX::XMFLOAT3 line3 = {corners[5].x, corners[2].y, offset};

	DebugRenderer::AddLine(line0, line1, color);
	DebugRenderer::AddLine(line2, line3, color);
}

template<typename T>
void HexagonTile<T>::DrawmX(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset) {
	DirectX::XMFLOAT2 corners[Hexagon::NUMBER_OF_SIDES];
	HexagonalGridLayout::GetHexCorners(*this, layout, &corners[0]);

	auto x1 = 0.5f*(corners[2].x + corners[3].x);
	auto x2 = 0.5f*(corners[1].x + corners[0].x);

	auto y1 = 0.5f*(corners[3].y + corners[4].y);
	auto y2 = 0.5f*(corners[1].y + corners[0].y);

	DirectX::XMFLOAT3 point0 = {x1, y2, offset};
	DirectX::XMFLOAT3 point1 = {x2, y2, offset};
	DirectX::XMFLOAT3 point2 = {x1, y1, offset};
	DirectX::XMFLOAT3 point3 = {x2, y1, offset};

	DebugRenderer::AddLine(point0, point3, color);
	DebugRenderer::AddLine(point1, point2, color);
}

template<typename T>
void HexagonTile<T>::DrawCheapFill(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset) {
	DrawX(layout, color, offset);
	DrawT(layout, color, offset);
	DrawmX(layout, color, offset);
}

template<typename T>
void HexagonTile<T>::DrawExpensiveFill(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset) {
	DirectX::XMFLOAT2 corners[Hexagon::NUMBER_OF_SIDES];
	HexagonalGridLayout::GetHexCorners(*this, layout, &corners[0]);

	auto x1 = 0.5f*(corners[2].x + corners[3].x);
	auto x2 = 0.5f*(corners[1].x + corners[0].x);

	auto y1 = 0.5f*(corners[3].y + corners[4].y);
	auto y2 = 0.5f*(corners[1].y + corners[0].y);

	auto x = 0.5f*(corners[2].x + corners[1].x);

	DirectX::XMFLOAT3 topmid = {x, corners[2].y, offset};
	DirectX::XMFLOAT3 bottommid = {x, corners[4].y, offset};

	DirectX::XMFLOAT3 midtopleftmid = {x1, y2, offset};
	DirectX::XMFLOAT3 midtoprightmid = {x2, y2, offset};
	DirectX::XMFLOAT3 midbottomleftmid = {x1, y1, offset};
	DirectX::XMFLOAT3 midbottomrightmid = {x2, y1, offset};

	DirectX::XMFLOAT3 right = {corners[0].x, corners[0].y, offset};
	DirectX::XMFLOAT3 topright = {corners[1].x, corners[1].y, offset};
	DirectX::XMFLOAT3 topleft = {corners[2].x, corners[2].y, offset};
	DirectX::XMFLOAT3 left = {corners[3].x, corners[3].y, offset};
	DirectX::XMFLOAT3 bottomleft = {corners[4].x, corners[4].y, offset};
	DirectX::XMFLOAT3 bottomright = {corners[5].x, corners[5].y, offset};


	DebugRenderer::AddLine(right, midtoprightmid, color);
	DebugRenderer::AddLine(right, topright, color);
	DebugRenderer::AddLine(right, topmid, color);
	DebugRenderer::AddLine(right, midtopleftmid, color);
	DebugRenderer::AddLine(right, topleft, color);
	DebugRenderer::AddLine(right, left, color);
	DebugRenderer::AddLine(right, midbottomleftmid, color);
	DebugRenderer::AddLine(right, bottomleft, color);
	DebugRenderer::AddLine(right, bottommid, color);
	DebugRenderer::AddLine(right, bottomright, color);
	DebugRenderer::AddLine(right, midbottomrightmid, color);

	DebugRenderer::AddLine(left, midtoprightmid, color);
	DebugRenderer::AddLine(left, topright, color);
	DebugRenderer::AddLine(left, topmid, color);
	DebugRenderer::AddLine(left, topleft, color);
	DebugRenderer::AddLine(left, midtopleftmid, color);
	DebugRenderer::AddLine(left, midbottomleftmid, color);
	DebugRenderer::AddLine(left, bottomleft, color);
	DebugRenderer::AddLine(left, bottommid, color);
	DebugRenderer::AddLine(left, bottomright, color);
	DebugRenderer::AddLine(left, midbottomrightmid, color);

	DebugRenderer::AddLine(bottommid, midtoprightmid, color);
	DebugRenderer::AddLine(bottommid, topright, color);
	DebugRenderer::AddLine(bottommid, topmid, color);
	DebugRenderer::AddLine(bottommid, topleft, color);
	DebugRenderer::AddLine(bottommid, midtopleftmid, color);
	DebugRenderer::AddLine(bottommid, midbottomleftmid, color);
	DebugRenderer::AddLine(bottommid, bottomleft, color);
	DebugRenderer::AddLine(bottommid, bottomright, color);
	DebugRenderer::AddLine(bottommid, midbottomrightmid, color);

	DebugRenderer::AddLine(topmid, midtoprightmid, color);
	DebugRenderer::AddLine(topmid, topright, color);
	DebugRenderer::AddLine(topmid, topleft, color);
	DebugRenderer::AddLine(topmid, midtopleftmid, color);
	DebugRenderer::AddLine(topmid, midbottomleftmid, color);
	DebugRenderer::AddLine(topmid, bottomleft, color);
	DebugRenderer::AddLine(topmid, bottomright, color);
	DebugRenderer::AddLine(topmid, midbottomrightmid, color);
}


template<typename T>
void HexagonTile<T>::DrawT(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset) {
	DirectX::XMFLOAT2 corners[Hexagon::NUMBER_OF_SIDES];
	HexagonalGridLayout::GetHexCorners(*this, layout, &corners[0]);

	auto x = 0.5f*(corners[2].x + corners[1].x);

	DirectX::XMFLOAT3 line0 = {x, corners[2].y, offset};
	DirectX::XMFLOAT3 line1 = {x, corners[4].y, offset};

	DirectX::XMFLOAT3 line2 = {corners[3].x, corners[0].y, offset};
	DirectX::XMFLOAT3 line3 = {corners[0].x, corners[3].y, offset};

	DebugRenderer::AddLine(line0, line1, color);
	DebugRenderer::AddLine(line2, line3, color);
}

template<typename T>
HexTile HexagonTile<T>::Round(HexDTile h) {
	int _q = (int) std::round(h.q);
	int _r = (int) std::round(h.r);
	int _s = (int) std::round(h.s);
	double q_diff = abs(_q - h.q);
	double r_diff = abs(_r - h.r);
	double s_diff = abs(_s - h.s);
	if(q_diff > r_diff && q_diff > s_diff) {
		_q = -_r - _s;
	} else if(r_diff > s_diff) {
		_r = -_q - _s;
	} else {
		_s = -_q - _r;
	}
	return HexTile(_q, _r, _s);
}

template<typename T>
inline HexDTile HexagonTile<T>::Lerp(HexTile a, HexTile b, double t) {
	return HexDTile(lerp(a.q, b.q, t), lerp(a.r, b.r, t), lerp(a.s, b.s, t));
}

template<typename T>
inline HexDTile HexagonTile<T>::Lerp(HexDTile a, HexDTile b, double t) {
	return HexDTile(lerp(a.q, b.q, t), lerp(a.r, b.r, t), lerp(a.s, b.s, t));
}

template<typename T>
std::vector<HexTile> HexagonTile<T>::DrawLineTo(HexTile* b) {
	int N = (int) DistanceFrom(b);
	std::vector<HexTile> results = {};
	double step = 1.0 / max(N, 1); //if b is this, N = 0
	for(int i = 0; i <= N; ++i) {
		results.push_back(Round(Lerp(*this, *b, step * i)));
	}
	return results;
}

template<typename T>
std::vector<HexTile> HexagonTile<T>::DrawSmoothLineTo(HexTile* b) {
	int N = (int) DistanceFrom(b);
	HexDTile push_this = plus(HexEpsilon, *this);
	HexDTile push_that = plus(HexEpsilon, *b);

	std::vector<HexTile> results = {};
	double step = 1.0 / max(N, 1); //if b is this, N = 0
	for(int i = 0; i <= N; ++i) {
		results.push_back(Round(Lerp(push_this, push_that, step * i)));
	}
	return results;
}

template<typename T>
std::vector<HexTile> HexagonTile<T>::SuperCover(HexTile* b) {
	int dx = abs(b->q - q);
	int dy = abs(b->r - r);

	HexTile tmp(q, r);

	int n = 1 + dx + dy;
	int x_inc = (b->q > q) ? 1 : -1;
	int y_inc = (b->r > r) ? 1 : -1;
	int error = dx - dy;
	dx *= 2;
	dy *= 2;

	std::vector<HexTile> results = {};

	for(; n > 0; --n) {
		results.push_back(tmp);

		if(error > 0) {
			tmp.q += x_inc;
			error -= dy;
		} else {
			tmp.r += y_inc;
			error += dx;
		}
	}

	return results;
}

template<typename T>
inline void HexagonTile<T>::RotateLeft() { *this = HexagonTile<T>(-s, -q, -r); } //use a swap?

template<typename T>
inline void HexagonTile<T>::RotateRight() { *this = HexagonTile<T>(-r, -s, -q); } //use a swap?

template<typename T>
T HexagonTile<T>::DistanceFrom(HexTile* b) { return (T) (std::max)({abs(q - b->q), abs(r - b->r), abs(s - b->s)}); } // = length of this - b
