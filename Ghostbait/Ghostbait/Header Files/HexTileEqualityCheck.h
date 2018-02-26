#pragma once
//I hate this
//find out why maps and sets aren't using the = operator inside HexTile correctly

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;
struct EqualComparator {
	bool operator()(const HexTile* lhs, const HexTile* rhs) const;
	bool operator()(const HexTile& lhs, const HexTile& rhs) const;
};
