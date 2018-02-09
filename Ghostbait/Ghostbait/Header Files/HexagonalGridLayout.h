#pragma once


struct Orientation {
	const double f0, f1, f2, f3;
	const double b0, b1, b2, b3;
	const double start_angle; // in multiples of 60 degrees
	Orientation(double f0_, double f1_, double f2_, double f3_, double b0_, double b1_, double b2_, double b3_, double start_angle_)
		: f0(f0_), f1(f1_), f2(f2_), f3(f3_),
		b0(b0_), b1(b1_), b2(b2_), b3(b3_),
		start_angle(start_angle_) {}

	Orientation(const Orientation& o) :f0(o.f0), f1(o.f1), f2(o.f2), f3(o.f3),
		b0(o.b0), b1(o.b1), b2(o.b2), b3(o.b3),
		start_angle(o.start_angle) {}

};

const double sqrt_3		  = 1.7320508075688772935274463415058723669428052538104;
const double sqrt_3_div_2 = 0.86602540378443864676372317075293618347140262690519;
const double sqrt_3_div_3 = 0.57735026918962576450914878050195745564760175127013;
const double _1_div_3	  = 0.33333333333333333333333333333333333333333333333333;
const double _2_div_3	  = 0.66666666666666666666666666666666666666666666666667;
const double GhostPI	  = 3.1415926535897932384626433832795028841971693993751;
const double pi_div_3	  = 1.047197551196597746154214461093167628065723133125;

const Orientation layout_pointy = Orientation(sqrt_3, sqrt_3_div_2, 0.0, 1.5, sqrt_3_div_3, -_1_div_3, 0.0, _2_div_3, 0.5);
const Orientation layout_flat = Orientation(1.5, 0.0, sqrt_3_div_2, sqrt_3, _2_div_3, 0.0, -_1_div_3, sqrt_3_div_3, 0.0);
 
#define OUT

template <typename T>
struct HexagonTile;

template <typename T>
union Point2 {
	T d[3];
	struct { T x, y, z; };
};

namespace DirectX {
	struct XMFLOAT2;
}

struct HexagonalGridLayout {
	const Orientation orientation;
	const Point2<float> size;
	const Point2<float> origin;

	HexagonalGridLayout() : orientation(layout_flat), size {10,10}, origin {0,0} {}
	HexagonalGridLayout(const Orientation orientation_, DirectX::XMFLOAT2 size_, DirectX::XMFLOAT2 origin_);

	//points won't line up exactly with the tile's center. to get the tile, round it
	static HexagonTile<double> HexagonalGridLayout::PointToHexagonTile(DirectX::XMFLOAT2 p, HexagonalGridLayout layout);

	//for drawing
	static DirectX::XMFLOAT2 GetHexCornerOffset(HexagonalGridLayout layout, int corner);

	static void GetHexCorners(HexagonTile<int> h, HexagonalGridLayout layout, OUT DirectX::XMFLOAT2* corners);
};

