#include "HexagonalGridLayout.h"
#include <DirectXMath.h>
#include "HexagonTile.h"

const double sqrt_3 = 1.7320508075688772935274463415058723669428052538104;
const double sqrt_3_div_2 = 0.86602540378443864676372317075293618347140262690519;
const double sqrt_3_div_3 = 0.57735026918962576450914878050195745564760175127013;
const double _1_div_3 = 0.33333333333333333333333333333333333333333333333333;
const double _2_div_3 = 0.66666666666666666666666666666666666666666666666667;
const double GhostPI = 3.1415926535897932384626433832795028841971693993751;
const double pi_div_3 = 1.047197551196597746154214461093167628065723133125;

const Orientation Orientation::Pointy = Orientation(sqrt_3, sqrt_3_div_2, 0.0, 1.5, sqrt_3_div_3, -_1_div_3, 0.0, _2_div_3, 0.5);
const Orientation Orientation::Flat = Orientation(1.5, 0.0, sqrt_3_div_2, sqrt_3, _2_div_3, 0.0, -_1_div_3, sqrt_3_div_3, 0.0);

const HexagonalGridLayout HexagonalGridLayout::FlatLayout = HexagonalGridLayout(Orientation::Flat, DirectX::XMFLOAT2(0.3f, 0.3f), DirectX::XMFLOAT2(0, 0));
const HexagonalGridLayout HexagonalGridLayout::PointyLayout = HexagonalGridLayout(Orientation::Pointy, DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT2(0, 0));

Orientation::Orientation(double _f0, double _f1, double _f2, double _f3, double _b0, double _b1, double _b2, double _b3, double _start_angle) :
	f0(_f0), f1(_f1), f2(_f2), f3(_f3),
	b0(_b0), b1(_b1), b2(_b2), b3(_b3),
	start_angle(_start_angle) {}

Orientation::Orientation(const Orientation& o) :
	f0(o.f0), f1(o.f1), f2(o.f2), f3(o.f3),
	b0(o.b0), b1(o.b1), b2(o.b2), b3(o.b3),
	start_angle(o.start_angle) {}

HexagonalGridLayout::HexagonalGridLayout(const Orientation _orientation, DirectX::XMFLOAT2 _size, DirectX::XMFLOAT2 _origin) :
	orientation(_orientation), size {_size.x, _size.y}, origin {_origin.x, _origin.y} {}

HexagonTile<double> HexagonalGridLayout::PointToHexagonTile(DirectX::XMFLOAT2 p, HexagonalGridLayout layout) {
	const Orientation& M = layout.orientation;
	DirectX::XMFLOAT2 pt = DirectX::XMFLOAT2((p.x - layout.origin.x) / layout.size.x,
		(p.y - layout.origin.y) / layout.size.y);
	double q = M.b0 * pt.x + M.b1 * pt.y;
	double r = M.b2 * pt.x + M.b3 * pt.y;
	return HexagonTile<double>(q, r);
}

DirectX::XMFLOAT2 HexagonalGridLayout::GetHexCornerOffset(HexagonalGridLayout layout, int corner) {
	//this doesn't work so well if you only want to draw certain edges. it won't line up with neighbors
	Point2<float> size = layout.size;
	double angle = pi_div_3 * (layout.orientation.start_angle + corner);
	return DirectX::XMFLOAT2(float(size.x * cos(angle)), float(size.y * sin(angle)));
}

void HexagonalGridLayout::GetHexCorners(HexTile h, HexagonalGridLayout layout, OUT DirectX::XMFLOAT2* corners) {
	DirectX::XMFLOAT2 center = h.Center(layout);
	for(int i = 0; i < Hexagon::NUMBER_OF_SIDES; ++i) {
		DirectX::XMFLOAT2 offset = GetHexCornerOffset(layout, i);
		corners[i] = DirectX::XMFLOAT2(center.x + offset.x, center.y + offset.y);
	}
}
