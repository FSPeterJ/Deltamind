#include "Heuristics.h"
#include "HexagonTile.h"

const float Heuristics::heuristicWeight = 1.2f;

float Heuristics::fast_sqrt(float x) {
	//http://bits.stephan-brumme.com/squareRoot.html
	unsigned int i = *(unsigned int*) &x;
	i += 127 << 23;
	i >>= 1;
	return *(float*) &i;
}

float Heuristics::ManhattanDistance(HexTile* a, HexTile* b) { return (float(std::abs(a->q - b->q)) + float(std::abs(a->r - b->r))) * heuristicWeight; }

float Heuristics::ChebyshevDistance(HexTile* a, HexTile* b) {
	float dx = float(std::abs(a->q - b->q)), dy = float(std::abs(a->r - b->r));
	return float(heuristicWeight * (dx + dy) - heuristicWeight * min(dx, dy));
}
float Heuristics::OctileDistance(HexTile* a, HexTile* b) {
	float dx = float(std::abs(a->q - b->q)), dy = float(std::abs(a->r - b->r));
	return float(heuristicWeight * (dx + dy) + (sqrt_2 - 2 * heuristicWeight) * min(dx, dy));
}
float Heuristics::EuclideanDistance(HexTile* a, HexTile* b) {
	float dx = float(std::abs(a->q - b->q)), dy = float(std::abs(a->r - b->r));
	return float(heuristicWeight * fast_sqrt(dx * dx + dy * dy));
}
