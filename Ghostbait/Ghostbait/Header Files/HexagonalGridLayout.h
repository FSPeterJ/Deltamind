#pragma once

const double GhostPI = 3.14159265358979323846;
#include <cmath>
#include <DirectXMath.h>
#include "HexagonTile.h"

struct Orientation {
	const double f0, f1, f2, f3;
	const double b0, b1, b2, b3;
	const double start_angle; // in multiples of 60�
	Orientation(double f0_, double f1_, double f2_, double f3_,
		double b0_, double b1_, double b2_, double b3_,
		double start_angle_)
		: f0(f0_), f1(f1_), f2(f2_), f3(f3_),
		b0(b0_), b1(b1_), b2(b2_), b3(b3_),
		start_angle(start_angle_) {}

	Orientation(const Orientation& o) :f0(o.f0), f1(o.f1), f2(o.f2), f3(o.f3),
		b0(o.b0), b1(o.b1), b2(o.b2), b3(o.b3),
		start_angle(o.start_angle) {}

};


const Orientation layout_pointy
= Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0,
	sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0,
	0.5);
const Orientation layout_flat
= Orientation(3.0 / 2.0, 0.0, sqrt(3.0) / 2.0, sqrt(3.0),
	2.0 / 3.0, 0.0, -1.0 / 3.0, sqrt(3.0) / 3.0,
	0.0);


struct HexagonalGridLayout {
	const Orientation orientation;
	const DirectX::XMFLOAT2 size;
	const DirectX::XMFLOAT2 origin;
		
	
	HexagonalGridLayout() : orientation(layout_flat), size {10,10}, origin {0,0} {}


	HexagonalGridLayout(const Orientation orientation_, DirectX::XMFLOAT2 size_, DirectX::XMFLOAT2 origin_)
		: orientation(orientation_), size(size_), origin(origin_) {}




	//points won't line up exactly with the tile's center. to get the tile, round it
	static HexDTile pixel_to_hex(HexagonalGridLayout layout, DirectX::XMFLOAT2 p) {
			const Orientation& M = layout.orientation;
			DirectX::XMFLOAT2 pt = DirectX::XMFLOAT2((p.x - layout.origin.x) / layout.size.x,
				(p.y - layout.origin.y) / layout.size.y);
			double q = M.b0 * pt.x + M.b1 * pt.y;
			double r = M.b2 * pt.x + M.b3 * pt.y;
			return HexDTile(q, r);
		}


	//for drawing
	static DirectX::XMFLOAT2 hex_corner_offset(HexagonalGridLayout layout, int corner) {
		DirectX::XMFLOAT2 size = layout.size;
		double angle = 2.0 * GhostPI * (layout.orientation.start_angle + corner) / 6;
		return DirectX::XMFLOAT2(float(size.x * cos(angle)), float(size.y * sin(angle)));
	}


	static std::vector<DirectX::XMFLOAT2> polygon_corners(HexagonalGridLayout layout, HexTile h) {
		std::vector<DirectX::XMFLOAT2> corners = {};
		DirectX::XMFLOAT2 center = h.Center(layout);
		for(int i = 0; i < 6; ++i) {
			DirectX::XMFLOAT2 offset = hex_corner_offset(layout, i);
			corners.push_back(DirectX::XMFLOAT2(center.x + offset.x, center.y + offset.y));
		}
		return corners;
	}



};

