#pragma once

struct Orientation {
	static const Orientation Pointy;
	static const Orientation Flat;

	const double f0, f1, f2, f3;
	const double b0, b1, b2, b3;
	const double start_angle; // in multiples of 60 degrees

	Orientation(double _f0, double _f1, double _f2, double _f3, double _b0, double _b1, double _b2, double _b3, double _start_angle);

	Orientation(const Orientation& o);
};

#define OUT

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;
typedef HexagonTile<double> HexDTile;

namespace DirectX { struct XMFLOAT2; }

template <typename T>
union Point2 { T d[2]; struct { T x, y; }; };

struct HexagonalGridLayout {

	static const HexagonalGridLayout FlatLayout;
	static const HexagonalGridLayout PointyLayout;

	const Orientation orientation;
	const Point2<float> size;
	const Point2<float> origin;

	HexagonalGridLayout() : orientation(Orientation::Flat), size {10,10}, origin {0,0} {}
	HexagonalGridLayout(const Orientation _orientation, DirectX::XMFLOAT2 _size, DirectX::XMFLOAT2 _origin);

	//points won't line up exactly with the tile's center. to get the tile, round it
	static HexDTile HexagonalGridLayout::PointToHexagonTile(DirectX::XMFLOAT2 p, HexagonalGridLayout layout);

	//for drawing
	static DirectX::XMFLOAT2 GetHexCornerOffset(HexagonalGridLayout layout, int corner);

	static void GetHexCorners(HexTile h, HexagonalGridLayout layout, OUT DirectX::XMFLOAT2* corners);
};
