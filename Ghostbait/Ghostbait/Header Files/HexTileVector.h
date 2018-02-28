#pragma once
#include <unordered_map>
#include "HexagonTile.h" //vector
#include "TraversalResult.h" //VisitedMap

struct HexagonalGridLayout;

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

class HexGrid;

namespace DirectX { struct XMFLOAT2; struct XMFLOAT3; }

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

	/// <summary>
	/// Determines whether this region contains the specified HexTile.
	/// </summary>
	/// <param name="v">The tile.</param>
	/// <returns>true if contains, otherwise false.</returns>
	bool contains(const HexTile& v);

	/// <summary>
	/// Finds the specified tile.
	/// </summary>
	/// <param name="v">The tile.</param>
	/// <returns>an iterator representing the find result.</returns>
	std::vector<HexTile>::iterator find(const HexTile& v);

	/// <summary>
	/// Removes the specified tile.
	/// </summary>
	/// <param name="v">The tile.</param>
	/// <returns>an iterator representing the remove result.</returns>
	std::vector<HexTile>::iterator remove(const HexTile& v);

	/// <summary>
	/// Adds a tile to this region.
	/// </summary>
	/// <param name="v">The tile.</param>
	void push_back(const HexTile& v);

	/// <summary>
	/// Adds a tile to this region.
	/// </summary>
	/// <param name="v">The tile.</param>
	void push_back(HexTile&& v);

	std::vector<HexTile>::iterator begin() { return data.begin(); }
	std::vector<HexTile>::iterator end() { return data.end(); }

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

	std::vector<HexTilePtr>::iterator remove(const HexTilePtr& v);

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

bool operator<(const HexPath&p, const HexPath&p2);
