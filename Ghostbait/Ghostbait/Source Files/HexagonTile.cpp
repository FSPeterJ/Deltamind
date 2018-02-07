#include "HexagonTile.h"
#include "DebugRenderer.h"
#include <DirectXMath.h>
#include "HexGrid.h"

template DirectX::XMFLOAT2 HexTile::Center(HexagonalGridLayout layout);
template void HexTile::Draw(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset);
template void HexTile::Cross(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset);
template std::vector<HexTile*> HexTile::GetTilesNStepsAway(HexGrid * grid, size_t n);
template std::vector<HexTile> HexTile::DrawLineTo(HexTile b);
template std::vector<HexTile> HexTile::DrawSmoothLineTo(HexTile b);
template std::vector<HexTile> HexTile::SuperCover(HexTile b);

bool CostComparator::operator()(const HexTile& lhs, const HexTile& rhs) const {
	return lhs.cost < rhs.cost;
}
bool CostComparator::operator()(const HexTile* lhs, const HexTile* rhs) const {
	return lhs->cost < rhs->cost;
}

template<typename T>
DirectX::XMFLOAT2 HexagonTile<T>::Center(HexagonalGridLayout layout) {
	const Orientation& M = layout.orientation;
	double x = (M.f0 * q + M.f1 * r) * layout.size.x;
	double y = (M.f2 * q + M.f3 * r) * layout.size.y;
	return DirectX::XMFLOAT2(float(x + layout.origin.x), float(y + layout.origin.y));
}

template<typename T>
void HexagonTile<T>::Draw(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset) {
	auto corners = HexagonalGridLayout::polygon_corners(layout, *this);

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
void HexagonTile<T>::Cross(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset) {
	auto corners = HexagonalGridLayout::polygon_corners(layout, *this);

	DirectX::XMFLOAT3 line0 = {corners[1].x, corners[4].y, offset};
	DirectX::XMFLOAT3 line1 = {corners[4].x, corners[1].y, offset};
	DirectX::XMFLOAT3 line2 = {corners[2].x, corners[5].y, offset};
	DirectX::XMFLOAT3 line3 = {corners[5].x, corners[2].y, offset};

	DebugRenderer::AddLine(line0, line1, color);
	DebugRenderer::AddLine(line2, line3, color);
}

template<typename T>
std::vector<HexTile> HexagonTile<T>::DrawLineTo(HexTile b) {
	int N = (int) DistanceFrom(b);
	std::vector<HexTile> results = {};
	double step = 1.0 / max(N, 1); //if b is this, N = 0
	for(int i = 0; i <= N; ++i) {
		results.push_back(Round(hex_lerp(*this, b, step * i)));
	}
	return results;
}

//use this if it's lerping to edges a lot
template<typename T>
std::vector<HexTile> HexagonTile<T>::DrawSmoothLineTo(HexTile b) {
	int N = (int) DistanceFrom(b);
	HexDTile push_this = add(*this, epsilon);
	HexDTile push_that = add(b, epsilon);

	std::vector<HexTile> results = {};
	double step = 1.0 / max(N, 1); //if b is this, N = 0
	for(int i = 0; i <= N; ++i) {
		results.push_back(Round(hex_lerp(push_this, push_that, step * i)));
	}
	return results;
}

template<typename T>
std::vector<HexTile> HexagonTile<T>::SuperCover(HexTile b) {
	int dx = abs(b.q - q);
	int dy = abs(b.r - r);

	HexTile tmp(q, r);

	int n = 1 + dx + dy;
	int x_inc = (b.q > q) ? 1 : -1;
	int y_inc = (b.r > r) ? 1 : -1;
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
std::vector<HexTile*> HexagonTile<T>::GetTilesNStepsAway(HexGrid * grid, size_t n) {
	std::vector<HexTile> results = {};

	//std::vector<HexTile*> tiles = grid.
}
