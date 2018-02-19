#pragma once

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

struct Heuristics {
	const static float heuristicWeight;

	static float fast_sqrt(float x);

	static float ManhattanDistance(HexTile* a, HexTile* b);
	static float ChebyshevDistance(HexTile* a, HexTile* b);
	static float OctileDistance(HexTile* a, HexTile* b);
	static float EuclideanDistance(HexTile* a, HexTile* b);
};
