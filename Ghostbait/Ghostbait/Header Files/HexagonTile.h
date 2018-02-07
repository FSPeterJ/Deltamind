#pragma once
#include <vector>

struct HexagonalGridLayout;

template<typename T>
struct HexagonTile;

class HexGrid;

typedef HexagonTile<int> HexTile;
typedef HexagonTile<double> HexDTile;
//typedef HexagonTile<float> HexFTile;

typedef unsigned char uint8;

namespace DirectX { struct XMFLOAT2;  struct XMFLOAT3; }

enum DIRECTION {
	Right = 0,
	TopRight,
	TopLeft,
	Left,
	BottomLeft,
	BottomRight
};

enum DIAGONAL {
	MidTopRight = 0,
	Top,
	MidTopLeft,
	MidBottomLeft,
	Bottom,
	MidBottomRight
};

struct CostComparator {
	bool operator()(const HexTile& lhs, const HexTile& rhs) const;
	bool operator()(const HexTile* lhs, const HexTile* rhs) const;
};

template<typename T>
struct HexagonTile {
	union {
		T v[3];
		struct { T q, r, s; };
	};

	float cost = 0;

	HexagonTile() : v {0,0,0} {}

	HexagonTile(const T& q_, const T& r_) : v {q_, r_, -q_ - r_} {}

	HexagonTile(const T& q_, const T& r_, const T& s_) : v {q_, r_, s_} {}

	HexagonTile<T> operator +( const HexagonTile<T>& b) const {
		return HexagonTile<T>(this->q + b.q , this->r + b.r , this->s + b.s);
	}

	bool operator ==(const HexagonTile<T>& b) {
		return q == b.q && r == b.r && s == b.s;
	}
	bool operator != (const HexagonTile<T>& b) {
		return !(*this == b);
	}

	bool operator ==(const HexagonTile<T>& b)const {
		return q == b.q && r == b.r && s == b.s;
	}
	bool operator != (const HexagonTile<T>& b) const {
		return !(*this == b);
	}

	void add(HexTile b) {
		q += b.q, r += b.r, s += b.s;
	}

	HexDTile add(HexTile a, HexDTile b) {
		return HexDTile((double) a.q + b.q, (double) a.r + b.r, (double) a.s + b.s);
	}

	HexTile sub(HexTile a, HexTile b) {
		return HexTile((int) a.q + b.q, (int) a.r + b.r, (int) a.s + b.s);
	}

	void sub(HexTile b) {
		q += b.q, r += b.r, s += b.s;
	}

	void mult(int scalar) {
		q *= scalar, r *= scalar, s *= scalar;
	}

	float Length() {
		return ((int) (abs(q) + abs(r) + abs(s)))*0.5f;
	}

	DirectX::XMFLOAT2 Center(HexagonalGridLayout layout);

	float DistanceFrom(HexTile b) {
		//return sub(b, *this).Length();
		//return sub(*this, b).Length();
		return std::(max) (abs(q - b.q), abs(a.r - b.r), abs(a.s - b.s))
	}

	HexTile Direction(DIRECTION dir) {
		return directions[(int) dir];
	}

	HexTile Neighbor(DIRECTION dir) {
		return *this + Direction(dir);
	}

	std::vector<HexTile*> Neighbors() {
		std::vector < HexTile*> neighbors = {};

		HexTile tmp;
		for(size_t i = 0; i < 5; ++i) {
			tmp = *this + Direction((DIRECTION) i);
			neighbors.push_back(&tmp);
		}

		return neighbors;
	}

	HexTile DiagonalNeighbor(DIAGONAL dir) {
		return *this + diagonals[dir];
	}

	void Draw(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);

	void Cross(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);

	HexTile Round(HexDTile h) {
		int q = (int) std::round(h.q);
		int r = (int) std::round(h.r);
		int s = (int) std::round(h.s);
		double q_diff = abs(q - h.q);
		double r_diff = abs(r - h.r);
		double s_diff = abs(s - h.s);
		if(q_diff > r_diff && q_diff > s_diff) {
			q = -r - s;
		} else if(r_diff > s_diff) {
			r = -q - s;
		} else {
			s = -q - r;
		}
		return HexTile(q, r, s);
	}

	float lerp(int a, int b, double t) {
		return float(a * (1 - t) + b * t); //gives better floating point precision than a + (b - a) * t
	}
	float lerp(double a, double b, double t) { return float(a * (1 - t) + b * t); }

	HexDTile hex_lerp(HexTile a, HexTile b, double t) {
		return HexDTile(
			lerp(a.q, b.q, t),
			lerp(a.r, b.r, t),
			lerp(a.s, b.s, t));
	}

	HexDTile hex_lerp(HexDTile a, HexDTile b, double t) {
		return HexDTile(
			lerp(a.q, b.q, t),
			lerp(a.r, b.r, t),
			lerp(a.s, b.s, t));
	}

	std::vector<HexTile> DrawLineTo(HexTile b);

	//use this if it's lerping to edges a lot
	std::vector<HexTile> DrawSmoothLineTo(HexTile b);

	std::vector<HexTile> SuperCover(HexTile b);

	void RotateLeft() {
		auto tmp = *this;
		q = -tmp.s;
		r = -tmp.q;
		s = -tmp.r;
	}

	void RotateRight() {
		auto tmp = *this;
		q = -tmp.r;
		r = -tmp.s;
		s = -tmp.q;
	}

	std::vector<HexTile*> GetTilesNStepsAway(HexGrid* grid, size_t n);
};

HexDTile epsilon(1e-6, 1e-6, -2e-6);

const std::vector<HexTile> directions = {
   HexTile(1, 0, -1), HexTile(1, -1, 0), HexTile(0, -1, 1),
   HexTile(-1, 0, 1), HexTile(-1, 1, 0), HexTile(0, 1, -1)
};

const std::vector<HexTile> diagonals = {
   HexTile(2, -1, -1), HexTile(1, -2, 1), HexTile(-1, -1, 2),
   HexTile(-2, 1, 1), HexTile(-1, 2, -1), HexTile(1, 1, -2)
};
