#pragma once
#include <unordered_map>
#include "HexagonTile.h" //vector

struct HexagonalGridLayout;

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

class HexGrid;

namespace DirectX { struct XMFLOAT2; struct XMFLOAT3; }

using VisitedMap = std::unordered_map<HexTile *const, HexTile*, std::hash<HexTile*>>; //if things break, put EqualComparator in its own class include it here and Grid

enum class ColorType {
	__X,
	__T,
	__mX,
	__CheapFill,
	__Outline,
};

class HexRegion {
	std::vector<HexTile> data;
public:
	HexTile * getData();
	HexRegion() {}
	HexRegion(std::vector<HexTile>& that);

	HexRegion& operator=(std::vector<HexTile>& that) {
		data.swap(that);
		return *this;
	}

	void clear() { data.clear(); }

	HexRegion& operator=(HexRegion& that) {
		data.swap(that.data);
		return *this;
	}

	void remove(const HexTile& v);

	void push_back(const HexTile& v);

	void push_back(HexTile&& v);

	HexTile& operator[](const size_t i) { return data[i]; }

	const HexTile& operator[](const size_t i) const { return data[i]; }

	size_t size() const noexcept { return data.size(); }

	void Color(HexagonalGridLayout *const layout, DirectX::XMFLOAT3 color, float offset, ColorType c);

	void Filter(HexGrid& grid);
};

class HexPath {
	typedef HexTile* HexTilePtr;
	std::vector<HexTilePtr> data;
public:
	HexTilePtr * getData();

	HexTilePtr goal();
	HexTilePtr start();

	HexPath() {}

	HexPath(std::vector<HexTilePtr>& that);

	HexPath& operator=(std::vector<HexTilePtr>& that) {
		data.swap(that);
		return *this;
	}

	void clear() { data.clear(); }

	HexPath& operator=(HexPath& that) {
		data.swap(that.data);
		return *this;
	}

	void remove(const HexTilePtr& v);

	void push_back(const HexTilePtr& v);

	void push_back(HexTilePtr&& v);

	HexTilePtr& operator[](const size_t i) { return data[i]; }

	const HexTilePtr& operator[](const size_t i) const { return data[i]; }

	size_t size() const noexcept { return data.size(); }

	float cost() const;

	void BuildPath(HexTilePtr const start, HexTilePtr const goal, VisitedMap& came_from);

	void BuildPathReverse(HexTilePtr const start, HexTilePtr const goal, VisitedMap& came_from);

	void Color(HexagonalGridLayout *const layout, DirectX::XMFLOAT3 color, float offset, ColorType c);
};
