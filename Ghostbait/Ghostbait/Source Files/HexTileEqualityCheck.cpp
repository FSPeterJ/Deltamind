#include "HexTileEqualityCheck.h"
#include "HexagonTile.h"

bool EqualComparator::operator()(const HexTile* lhs, const HexTile* rhs) const { return *lhs == *rhs; }
bool EqualComparator::operator()(const HexTile& lhs, const HexTile& rhs) const { return lhs == rhs; }
