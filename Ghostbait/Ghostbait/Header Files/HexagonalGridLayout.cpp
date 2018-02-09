#include "HexagonalGridLayout.h"


#include <cmath>
#include <DirectXMath.h>
#include "HexagonTile.h"


HexagonalGridLayout::HexagonalGridLayout(const Orientation orientation_, DirectX::XMFLOAT2 size_, DirectX::XMFLOAT2 origin_) :
	orientation(orientation_), size {size_.x, size_.y}, origin {origin_.x, origin_.y} {}

//points won't line up exactly with the tile's center. to get the tile, round it
 HexagonTile<double> HexagonalGridLayout::PointToHexagonTile(DirectX::XMFLOAT2 p, HexagonalGridLayout layout) {
	const Orientation& M = layout.orientation;
	DirectX::XMFLOAT2 pt = DirectX::XMFLOAT2((p.x - layout.origin.x) / layout.size.x,
		(p.y - layout.origin.y) / layout.size.y);
	double q = M.b0 * pt.x + M.b1 * pt.y;
	double r = M.b2 * pt.x + M.b3 * pt.y;
	return HexagonTile<double>(q, r);
}

 DirectX::XMFLOAT2 HexagonalGridLayout::GetHexCornerOffset(HexagonalGridLayout layout, int corner) {
	 Point2<float> size = layout.size;
	 double angle = pi_div_3 * (layout.orientation.start_angle + corner);
	 return DirectX::XMFLOAT2(float(size.x * cos(angle)), float(size.y * sin(angle)));
 }

void HexagonalGridLayout::GetHexCorners(HexagonTile<int> h, HexagonalGridLayout layout, DirectX::XMFLOAT2* corners) {
	 DirectX::XMFLOAT2 center = h.Center(layout);
	 for(int i = 0; i < 6; ++i) {
		 DirectX::XMFLOAT2 offset = GetHexCornerOffset(layout, i);
		 corners[i] = DirectX::XMFLOAT2(center.x + offset.x, center.y + offset.y);
	 }
 }
