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

#define max(a,b) (((a) > (b)) ? (a) : (b))

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

	void swap(HexagonTile<T>& b) {
		std::swap(q, b.q);
		std::swap(r, b.r);
		std::swap(s, b.s);
		std::swap(v, b.v);
		std::swap(cost, b.cost);
	}

	HexagonTile<T>& operator=(HexagonTile<T> b) {
		swap(b);
		return *this;
	}

	HexagonTile<T> operator+=(const HexagonTile<T>& b) {
		q += b.q; r += b.r; s += b.s;
		return *this;
	}

	HexDTile plus_equal(const HexDTile& b) {
		return HexDTile(q + b.q, r + b.r, s + b.s);
	}

	HexagonTile<T> operator-=(const HexagonTile<T>& b) {
		q -= b.q; r -= b.r; s -= b.s;
		return *this;
	}
	HexagonTile<T> operator*=(const T& b) {
		q *= b; r *= b; s *= b;
		return *this;
	}

	float Length() { return ((int) (abs(q) + abs(r) + abs(s)))*0.5f; }

	DirectX::XMFLOAT2 Center(HexagonalGridLayout layout);

	T DistanceFrom(HexTile* b);

	inline HexTile Direction(DIRECTION dir) { return directions[(int) dir]; }

	HexTile Neighbor(DIRECTION dir) { return *this + Direction(dir); }

	std::vector<HexTile> Neighbors() {
		std::vector<HexTile> neighbors = {};

		for(size_t i = 0; i < 5; ++i) {
			neighbors.push_back(*this + Direction((DIRECTION) i));
		}

		return neighbors;
	}

	HexTile DiagonalNeighbor(DIAGONAL dir) { return *this + diagonals[dir]; }

	void Draw(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);

	void Cross(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);

	HexTile Round(HexDTile h) {
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

	float lerp(int a, int b, double t) { return float(a * (1 - t) + b * t); } //gives better floating point precision than a + (b - a) * t

	float lerp(double a, double b, double t) { return float(a * (1 - t) + b * t); }

	HexDTile Lerp(HexTile a, HexTile b, double t) {
		return HexDTile(lerp(a.q, b.q, t), lerp(a.r, b.r, t), lerp(a.s, b.s, t));
	}

	HexDTile Lerp(HexDTile a, HexDTile b, double t) {
		return HexDTile(lerp(a.q, b.q, t), lerp(a.r, b.r, t), lerp(a.s, b.s, t));
	}

	std::vector<HexTile> DrawLineTo(HexTile* b);

	//use this if it's lerping to edges a lot
	std::vector<HexTile> DrawSmoothLineTo(HexTile* b);

	std::vector<HexTile> SuperCover(HexTile* b);

	void RotateLeft() { *this = HexagonTile<T>(-s, -q, -r); } //use a swap?

	void RotateRight() { *this = HexagonTile<T>(-r, -s, -q); }
};

template<typename T>
inline HexagonTile<T> operator+(HexagonTile<T>& a, const HexagonTile<T>& b) {
	a += b;
	return a;
}

inline HexDTile plus(const HexDTile& a, HexTile& b) {
	return b.plus_equal(a);
}

template<typename T>
inline HexagonTile<T> operator-(HexagonTile<T>& a, const HexagonTile<T>& b) {
	return a -= b, a;
}

template<typename T>
inline HexagonTile<T> operator*(HexagonTile<T>& a, const float& b) {
	a *= b;
	return a;
}

template<typename T>
inline bool operator ==(const HexagonTile<T>& a, const HexagonTile<T>& b) {
	return a.q == b.q && a.r == b.r && a.s == b.s;
}

template<typename T>
inline bool operator != (const HexagonTile<T>& a, const HexagonTile<T>& b) {
	return !(a == b);
}

const std::vector<HexTile> directions = {
   HexTile(1, 0, -1), HexTile(1, -1, 0), HexTile(0, -1, 1),
   HexTile(-1, 0, 1), HexTile(-1, 1, 0), HexTile(0, 1, -1)
};

const std::vector<HexTile> diagonals = {
   HexTile(2, -1, -1), HexTile(1, -2, 1), HexTile(-1, -1, 2),
   HexTile(-2, 1, 1), HexTile(-1, 2, -1), HexTile(1, 1, -2)
};

//struct CostComparator {
//	bool operator()(const HexTile& lhs, const HexTile& rhs) const ;
//	bool operator()(const HexTile* lhs, const HexTile* rhs) const ;
//};

const HexDTile HexEpsilon(1e-6, 1e-6, -2e-6);
