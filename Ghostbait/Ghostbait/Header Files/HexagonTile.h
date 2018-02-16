#pragma once
#include <vector>

struct HexagonalGridLayout;

template<typename T>
struct HexagonTile;

typedef HexagonTile<int> HexTile;
typedef HexagonTile<double> HexDTile;

typedef unsigned char uint8;

namespace DirectX { struct XMFLOAT2;  struct XMFLOAT3; }
namespace Hexagon { const unsigned NUMBER_OF_SIDES = 6; }

enum class NEIGHBOR_DIRECTION {
	Right = 0,
	TopRight,
	TopLeft,
	Left,
	BottomLeft,
	BottomRight
};

enum class NEIGHBOR_DIAGONAL {
	MidTopRight = 0,
	Top,
	MidTopLeft,
	MidBottomLeft,
	Bottom,
	MidBottomRight
};

template<typename T>
struct HexagonTile {
private:
	friend class HexGrid;
	void * operator new (size_t sz) { return ::operator new(sz); }
	void * operator new (size_t sz, void* ad) { return ::operator new(sz, ad); }
	void * operator new[](size_t sz) { return ::operator new[](sz); }
	void   operator delete (void * ad) { return ::operator delete(ad); }
	void   operator delete[](void* ad) { return ::operator delete[](ad); }

	float lerp(int a, int b, double t) { return float(a * (1 - t) + b * t); } //gives better floating point precision than a + (b - a) * t

	float lerp(double a, double b, double t) { return float(a * (1 - t) + b * t); }

	void swap(HexagonTile<T>& b) {
		std::swap(q, b.q);
		std::swap(r, b.r);
		std::swap(s, b.s);
		std::swap(v, b.v); //needed?
		std::swap(weight, b.weight);
	}

	static const std::vector<HexTile> directions;
	static const std::vector<HexTile> diagonals;

public:
	union { T v[3]; struct { T q, r, s; }; };

	float weight = 0.0f;

	HexagonTile() : v {0,0,0} {}

	HexagonTile(const T& q_, const T& r_) : v {q_, r_, -q_ - r_} {}

	HexagonTile(const T& q_, const T& r_, const T& s_) : v {q_, r_, s_} {}

	HexagonTile<T>& operator=(HexagonTile<T> b);

	HexagonTile<T> operator+=(const HexagonTile<T>& b);

	HexagonTile<T> operator-=(const HexagonTile<T>& b);

	HexagonTile<T> operator*=(const T& b);

	const float Length() const;

	DirectX::XMFLOAT2 Center(HexagonalGridLayout layout);

	T DistanceFrom(HexTile* b);

	inline HexTile Direction(NEIGHBOR_DIRECTION dir) { return directions[(int) dir]; }

	inline HexTile Diagonal(NEIGHBOR_DIAGONAL dir) { return diagonals[(int) dir]; }

	HexTile Neighbor(NEIGHBOR_DIRECTION dir) { return *this + Direction(dir); }

	HexTile DiagonalNeighbor(NEIGHBOR_DIAGONAL dir) { return *this + Diagonal(dir); }

	std::vector<HexTile> Neighbors();

	void RotateLeft();

	void RotateRight();

	static HexTile Round(HexDTile h);

	std::vector<HexTile> DrawLineTo(HexTile* b);

	//use this if it's lerping to edges a lot
	std::vector<HexTile> DrawSmoothLineTo(HexTile* b);

	std::vector<HexTile> SuperCover(HexTile* b);

	void Draw(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);

	void DrawX(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);
	void DrawmX(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);
	void DrawCheapFill(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);
	void DrawExpensiveFill(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);

	void DrawT(HexagonalGridLayout layout, DirectX::XMFLOAT3 color, float offset = 0);

	inline HexDTile Lerp(HexTile a, HexTile b, double t);

	inline HexDTile Lerp(HexDTile a, HexDTile b, double t);

	HexDTile plus_equal(const HexDTile& b) { //u g l y
		return HexDTile(q + b.q, r + b.r, s + b.s);
	}
};

const double sqrt_2 = 1.414213562373095048801688724209698078569671875377;
#define min(a,b) (((a) < (b)) ? (a) : (b))

namespace Heuristics { //put this in grid maybe?
	const float heuristicWeight = 1.2f;

	static float fast_sqrt(float x) {
		//http://bits.stephan-brumme.com/squareRoot.html
		unsigned int i = *(unsigned int*) &x;
		i += 127 << 23;
		i >>= 1;
		return *(float*) &i;
	}


	inline static float ManhattanDistance(HexTile* a, HexTile* b) { return (float(std::abs(a->q - b->q)) + float(std::abs(a->r - b->r))) * heuristicWeight; }

	inline static float ChebyshevDistance(HexTile* a, HexTile* b) {
		float dx = float(std::abs(a->q - b->q)), dy = float(std::abs(a->r - b->r));
		return float(heuristicWeight * (dx + dy) - heuristicWeight * min(dx, dy));
	}
	inline static float OctileDistance(HexTile* a, HexTile* b) {
		float dx = float(std::abs(a->q - b->q)), dy = float(std::abs(a->r - b->r));
		return float(heuristicWeight * (dx + dy) + (sqrt_2 - 2 *heuristicWeight) * min(dx, dy));
	}
	inline static float EuclideanDistance(HexTile* a, HexTile* b) {
		float dx = float(std::abs(a->q - b->q)), dy = float(std::abs(a->r - b->r));
		return float(heuristicWeight * fast_sqrt(dx * dx + dy * dy));
	}

}

static const HexDTile HexEpsilon(1e-6, 1e-6, -2e-6);

template<typename T>
inline HexagonTile<T> operator+(const HexagonTile<T>& a, const HexagonTile<T>& b) {
	return HexagonTile<T>(a.q + b.q, a.r + b.r, a.s + b.s);
}

inline HexDTile plus(const HexDTile& a, HexTile& b) {
	return b.plus_equal(a);
}

template<typename T>
inline HexagonTile<T> operator-(const HexagonTile<T>& a, const HexagonTile<T>& b) {
	return HexagonTile<T>(a.q - b.q, a.r - b.r, a.s - b.s);
}

template<typename T>
inline HexagonTile<T> operator*(const HexagonTile<T>& a, const float& b) {
	return HexagonTile<T>(a.q *b, a.r *b, a.s *b);
}

template<typename T>
inline bool operator ==(const HexagonTile<T>& a, const HexagonTile<T>& b) {
	return a.q == b.q && a.r == b.r;// && a.s == b.s;
}

template<typename T>
inline bool operator != (const HexagonTile<T>& a, const HexagonTile<T>& b) {
	return !(a == b);
}
