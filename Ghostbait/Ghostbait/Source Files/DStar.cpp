#include "DStar.h"
#include "PathPlanner.h"
#include "HexGrid.h"
#include "HexagonTile.h"
#include "HexTileVector.h"
#include "Console.h"
using namespace Common;

#include<algorithm>

#define PrintParentConnnection(t, p) "Tile (" << t->q << ", " << t->r << ") -> ParentTile (" << p->q << ", " << p->r << ")"
#define PrintParentConnnectionNull(t) "Tile (" << t->q << ", " << t->r << ") -> ParentTile (null)"
#define PrintTileInfo(t, g, rhs, p) "Tile (" << t->q << ", " << t->r << "), c(" << GetCost(t) << "), g(" << g << "), rhs(" << rhs << ") ParentTile (" << p->q << ", " << p->r << ")"
#define PrintTileInfoNull(t, g, rhs) "Tile (" << t->q << ", " << t->r << "), c(" << GetCost(t) << "), g(" << g << "), rhs(" << rhs << ") ParentTile (NULL)"
#define PrintTile(t) "Tile (" << t->q << ", " << t->r << "), c(" << GetCost(t) << "), g(" << GetGVal(t) << "), rhs(" << GetRHS(t) << ") ParentTile (" << parent[t]->q << ", " << parent[t]->r << ")"

#pragma region DStarCommon

DStarCommon::DStarCommon(HexTile **const _start, HexTile **const _goal, HexTile **const _next, HexGrid *const _grid, std::size_t _perception) : start(_start), goal(_goal), next(_next), grid(_grid), perceptionRange(_perception), km(0.0f) {}

DStarCommon& DStarCommon::operator=(DStarCommon& _other) {
	grid = _other.grid;
	start = _other.start;
	goal = _other.goal;
	next = _other.next;
	open.swap(_other.open);

	knownTiles.swap(_other.knownTiles);
	changedTiles.swap(_other.changedTiles);
	km = _other.km;
	perceptionRange = _other.perceptionRange;
	return *this;
}

void DStarCommon::ForEachPredessor(HexTile*const tile, std::function<void(HexTile*const)> exec, UnionType includeSelf) {
	//pred = non blocked neighbors
	for (auto& element : tile->Neighbors()) {
		HexTile* neighbor = grid->GetTileExact(element);
		if (!neighbor || (grid->IsBlocked(neighbor) && GetGVal(tile) == tile->weight)) { continue; } 

		exec(neighbor);
	}
	if (includeSelf == UnionSelf) { exec(tile); }//&& !(grid->IsBlocked(tile) && grid->GetCostDelta().count(tile) == 0)) { exec(tile); }//Not sure if the union with self should check if itself is blocked or not
}

void DStarCommon::ForEachSuccessor(HexTile*const tile, std::function<void(HexTile*const)> exec, UnionType includeSelf) {
	//succ = if you blocked, no res, else return all
	if (grid->IsBlocked(tile) && GetGVal(tile) == tile->weight) { return; }
		
	for (auto& element : tile->Neighbors()) {
		HexTile* neighbor = grid->GetTileExact(element);
		if (!neighbor) { continue; }
		exec(neighbor);
	}
	if (includeSelf == UnionSelf) { exec(tile); }
}

float DStarCommon::GetMinimumFrom(const SearchType neighbors, HexTile*const tile, std::function<float(HexTile*const)> comparer, UnionType includeSelf) {
	float minmin = grid->BlockWeight();

	void(DStarCommon::*each)(HexTile*const, std::function<void(HexTile*const)>, UnionType) = ((neighbors == PredessorsOf) ? &DStarCommon::ForEachPredessor : &DStarCommon::ForEachSuccessor);

	(this->*each)(tile, [&](HexTile*const sPrime) {
		float minposib = comparer(sPrime);
		minmin = PathPlanner::ClampInfinity(min(minposib, minmin));
	}, includeSelf);

	return minmin;
}

HexTile* DStarCommon::GetMinimumTileFrom(const SearchType neighbors, HexTile*const tile, std::function<float(HexTile*const)> comparer, UnionType includeSelf) {
	HexTile* minTile = nullptr;
	float minmin = grid->BlockWeight() + 1.0f;

	void(DStarCommon::*each)(HexTile*const, std::function<void(HexTile*const)>, UnionType) = ((neighbors == PredessorsOf) ? &DStarCommon::ForEachPredessor : &DStarCommon::ForEachSuccessor);

	(this->*each)(tile, [&](HexTile*const sPrime) {
		float minposib = PathPlanner::ClampInfinity(comparer(sPrime));
		if (minposib < minmin) {
			minmin = minposib;
			minTile = sPrime;
		}
	}, includeSelf);

	return minTile;
}

FloatPair DStarCommon::CalculateKey(HexTile *const tile) {
	return { PathPlanner::ClampInfinity(min(GetGVal(tile), GetRHS(tile)) + PathPlanner::heuristicFunction(tile, *goal) + km), min(GetGVal(tile), GetRHS(tile)) };
}

float DStarCommon::GetGVal(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), 1.0f };
	}
	return knownTiles[tile].g;
}

float DStarCommon::GetRHS(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), 1.0f };
	}
	return knownTiles[tile].rhs;
}

float DStarCommon::GetCost(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), 1.0f };
	}
	return knownTiles[tile].cost;
}

float DStarCommon::GetGValInit(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), tile->weight };
	}
	return knownTiles[tile].g;
}

float DStarCommon::GetRHSInit(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), tile->weight };
	}
	return knownTiles[tile].rhs;
}

float DStarCommon::GetCostInit(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), tile->weight };
	}
	return knownTiles[tile].cost;
}

void DStarCommon::SetGVal(HexTile* tile, float value) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), 1.0f };
	}
	knownTiles[tile].g = value;
}

void DStarCommon::SetRHS(HexTile* tile, float value) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), 1.0f };
	}
	knownTiles[tile].rhs = value;
}

void DStarCommon::SetCost(HexTile* tile, float value) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), 1.0f };
	}
	knownTiles[tile].cost = value;
}

void DStarCommon::UpdateOpenList(HexTile *const tile) {
	//if (parent[tile]) Console::Write << PrintTileInfo(tile, GetGVal(tile], GetRHS(tile], parent[tile]);
	//else Console::Write << PrintTileInfoNull(tile, GetGVal(tile], GetRHS(tile]);
	if (!PathPlanner::EpsilonIsEqual(GetGVal(tile), GetRHS(tile))) {
		//Console::WriteLine << " Tile was Inconsistent";
		if (open.contains(tile)) {
			open.update(tile, CalculateKey(tile));
		}
		else {
			open.insert(tile, CalculateKey(tile));
		}
	}
	else if (PathPlanner::EpsilonIsEqual(GetGVal(tile), GetRHS(tile)) && open.contains(tile)) {
		//Console::WriteLine << " Tile was Consistent";
		open.remove(tile);
	}
}

//HexTile* DStarCommon::GetNextTileInPath() {
//	return next;
//}

#pragma endregion

#pragma region DStarLite

void DStarLite::ComputeShortestPath() {
	while (!open.empty() && (open.front().second < CalculateKey(*start) || GetRHS(*start) > GetGVal(*start))) {
		HexTile* u = open.front().first;
		FloatPair kold = open.front().second;
		FloatPair knew = CalculateKey(u);
		if (kold < knew) {
			open.update(u, knew);
		}
		else if (GetGVal(u) > GetRHS(u)) {
			knownTiles[u].g = knownTiles[u].rhs;
			open.remove(u);

			ForEachPredessor(u, [=](HexTile*const neighbor) {
				if (neighbor != *goal) {
					knownTiles[neighbor].rhs = PathPlanner::ClampInfinity((min(GetRHS(neighbor), GetCost(u) + GetGVal(u))));
				}
				UpdateOpenList(neighbor);
			});

		}
		else {
			float gold = GetGVal(u);
			knownTiles[u].g = grid->BlockWeight();

			ForEachPredessor(u, [=](HexTile*const neighbor) {
				if (PathPlanner::EpsilonIsEqual(GetRHS(neighbor), PathPlanner::ClampInfinity(GetCost(u) + gold))) {
					if (neighbor != *goal) {
						knownTiles[neighbor].rhs = GetMinimumFrom(SuccessorsOf, neighbor, [=](HexTile*const prime) {return GetCost(prime) + GetGVal(prime); });
					}
				}
				UpdateOpenList(neighbor);
			}, UnionSelf);

		}
	}
	//Replan();
}

DStarLite::DStarLite(HexGrid *const _grid, HexTile **const _start, HexTile **const _goal, HexTile **const _next, std::size_t _perception) :
	last(*_start), DStarCommon(_start, _goal, _next, _grid, _perception) {

	//this may not be neccessary
	//grid->ForEach([=](HexTile*const tile) {GetRHS(tile] = GetGVal(tile] = grid->BlockWeight(); });

	SetRHS(*goal, 0.0f);

	open.insert(*goal, std::make_pair(PathPlanner::heuristicFunction(*start, *goal), 0.0f));

	ComputeShortestPath();
}

DStarLite& DStarLite::operator=(DStarLite& _other) {
	last = _other.last;
	curNode = _other.curNode;
	DStarCommon::operator=(_other);
	return *this;
}

void DStarLite::Update() {//ran on thread
			   //	while(start != goal) {
	if (GetRHS(*start) == grid->BlockWeight()) {
		//there is no known path
		Console::WriteLine << "There's no PATH for MEE!!!!!";
		return;
	}

	//find next node on path
	HexTile* nextNode = GetMinimumTileFrom(SuccessorsOf, *start, [=](HexTile*const sPrime) { return GetCost(sPrime) + GetGVal(sPrime); });
	if (nextNode) { *start = nextNode; }

	//move enemy to start
	*next = *start; //??

	//scan graph for changed edge costs
	if (changedTiles.size()) { //if any edge costs changed
		//HexRegion range = grid->GetTilesNStepsAway(start, 3);
		HexRegion range = grid->Spiral(*start, perceptionRange);
		//for all directed edges
		for (HexTile& n : range /* grid->GetTilesNStepsAway(start, 3)*/) {
			HexTile* neighbor = grid->GetTileExact(n);
			//with changed edge costs
			if (changedTiles.count(neighbor)) { continue; }
			km = km + PathPlanner::heuristicFunction(last, *start);
			last = *start;

			float c_old = GetCost(neighbor);
			changedTiles.erase(neighbor);
			knownTiles[neighbor].cost = neighbor->weight;

			ForEachPredessor(neighbor, [=](HexTile* const pred) {
				if (c_old > neighbor->weight) {
					if (pred != *goal) {
						knownTiles[pred].rhs = PathPlanner::ClampInfinity(min(GetRHS(pred), neighbor->weight + GetGVal(neighbor)));
					}
				}
				else if (PathPlanner::EpsilonIsEqual(GetRHS(pred), PathPlanner::ClampInfinity(c_old + GetGVal(neighbor)))) {
					if (pred != *goal) {
						knownTiles[pred].rhs = GetMinimumFrom(SuccessorsOf, pred, [=](HexTile*const prime) {return prime->weight + GetGVal(prime); });
					}
				}
				UpdateOpenList(pred);
			});
		}
		ComputeShortestPath();
	}
}
//}

#pragma endregion

#pragma region MTDStarLite

//
//	HexTile* GetParent(HexTile* const tile) {
//		HexTile* me = (parent.find(tile) == parent.end()) ? (parent[tile] = nullptr) : parent[tile];
//		//if (parent.find(tile) == parent.end()) { parent[tile] = nullptr; }
//		return me;//parent[tile];
//	}
void MTDStarLite::ForEachInSearchTreeButNotSubtreeRootedAt(HexTile*const tile, NeighborFunction exec) {
	//figure what to put here
	HexTile* nextParent;
	std::unordered_set<HexTile*> traveled;
	for (auto& element : parent) {
		if (element.first == tile) continue;
		nextParent = element.second;
		traveled.clear();
		while (nextParent) {
			if (nextParent == tile) break;
			if (traveled.count(nextParent) > 0) parent[nextParent] = nullptr;
			traveled.insert(nextParent);
			//if (parent.find(nextParent) == parent.end()) {
			//	parent[nextParent] = nullptr;
			//}
			nextParent = parent[nextParent];
		}
		if (nextParent) continue;
		/*while (traveled.size()) {
		parent[traveled.back()] = nullptr;
		traveled.pop_back();
		}*/
		exec(element.first);
	}
}

void MTDStarLite::BasicDeletion(HexTile*const oldStart) {
	//Console::WriteLine << "Inside Basic Deletion";
	parent[*start] = nullptr;
	//Console::WriteLine << PrintParentConnnectionNull(start);
	knownTiles[oldStart].rhs = GetMinimumFrom(PredessorsOf, oldStart, [=](HexTile*const prime) { return GetGVal(prime) + GetCost(oldStart); });
	if (PathPlanner::EpsilonIsEqual(GetRHS(oldStart), grid->BlockWeight())) {
		parent[oldStart] = nullptr;
		//Console::WriteLine << PrintParentConnnectionNull(oldStart);
	}
	else {
		parent[oldStart] = GetMinimumTileFrom(PredessorsOf, oldStart, [=](HexTile*const prime) { return GetGVal(prime) + GetCost(oldStart); });
		//Console::WriteLine << PrintParentConnnection(oldStart, parent[oldStart]);
	}
	UpdateOpenList(oldStart);
}

void MTDStarLite::ComputeCostMinimalPath() {
	//Console::WriteLine << "Computing Cost Minimal Path";
	HexTile* cachedStart = *start;
	HexTile* cachedGoal = *goal;

	int iterations = 0;
	int maxIter = (int)PathPlanner::heuristicFunction(cachedStart, cachedGoal) * 15 + 30;
	bool hasPath = true;
	while (!open.empty() && (open.front().second < CalculateKey(cachedGoal) || GetRHS(cachedGoal) > GetGVal(cachedGoal))) {
		if (iterations++ > maxIter) {
			Console::WriteLine << "Max iterations reached, NOT SEARCHING ANY FURTHER";
			hasPath = false;
			break;
		}

		HexTile* u = open.front().first;
		FloatPair kold = open.front().second;
		FloatPair knew = CalculateKey(u);

		if (kold < knew) {
			open.update(u, knew);
		}
		else if (GetGVal(u) > GetRHS(u)) {
			knownTiles[u].g = knownTiles[u].rhs;
			open.remove(u);
			//if (parent[u]) Console::Write << PrintTileInfo(u, GetGVal(u), GetRHS(u), parent[u]);
			//else Console::Write << PrintTileInfoNull(u, GetGVal(u), GetRHS(u));
			//Console::WriteLine << " Tile was Over-consistent";
			//Console::WriteLine << " Updating susccessor neighbors...";

			ForEachSuccessor(u, [=](HexTile*const neighbor) {
				if (neighbor != cachedStart && (GetRHS(neighbor) > GetGVal(u) + GetCost(neighbor))) {
					parent[neighbor] = u;
					//Console::WriteLine << PrintParentConnnection(neighbor, u);
					knownTiles[neighbor].rhs = PathPlanner::ClampInfinity(GetGVal(u) + GetCost(neighbor));
					UpdateOpenList(neighbor);
				}
			});
		}
		else {
			knownTiles[u].g = grid->BlockWeight();
			//if (parent[u]) Console::Write << PrintTileInfo(u, GetGVal(u), GetRHS(u), parent[u]);
			//else Console::Write << PrintTileInfoNull(u, GetGVal(u), GetRHS(u));
			//Console::WriteLine << "Tile was Under-consistent";
			//Console::WriteLine << " Checking susccessor neighbors...";
			ForEachSuccessor(u, [=](HexTile*const neighbor) {
				if (neighbor != cachedStart && parent[neighbor] == u) {
					knownTiles[neighbor].rhs = GetMinimumFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGVal(prime) + GetCost(neighbor); });

					if (PathPlanner::EpsilonIsEqual(GetRHS(neighbor), grid->BlockWeight())) {
						parent[neighbor] = nullptr;
						//Console::WriteLine << "Path through here is Blocked";
						//Console::WriteLine << PrintParentConnnectionNull(neighbor);
					}
					else {
						HexTile* minTile = GetMinimumTileFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGVal(prime) + GetCost(neighbor); });
						//Console::WriteLine << "Finding new Path from here";
						parent[neighbor] = minTile; /*Console::WriteLine << PrintParentConnnection(neighbor, minTile);*/
													//parent[neighbor] = minTile;
					}
				}
				UpdateOpenList(neighbor);
			}, UnionSelf);
		}
	}
	//Console::WriteLine << "End Calculation";

	if (hasPath) {
		path.clear();
		//Console::WriteLine << "Begin building path";
		path.BuildPath(cachedStart, cachedGoal, parent); //might need to BuildPathReverse?
		*next = path.Next(cachedStart);
		//Console::WriteLine << "Path built";
		//for (int i = 0; i < path.size(); ++i) {
		//	HexTile* pathNode = path[i];
		//	if (parent[pathNode]) Console::WriteLine << PrintTileInfo(pathNode, GetGVal(pathNode), GetRHS(pathNode), parent[pathNode]);
		//	else Console::WriteLine << PrintTileInfoNull(pathNode, GetGVal(pathNode), GetRHS(pathNode));
		//}
	}
	else {
		/*float closest = grid->BlockWeight() + 1;
		float temp = 0;
		HexTile* nextClosest = nullptr;
		HexRegion ring;
		for (int range = (int)perceptionRange * 2; range > 0; --range) {
			ring = grid->Ring(start, range);
			for (int region = 0; region < ring.size(); ++region) {
				if (grid->IsBlocked(&ring[region])) continue;
				temp = PathPlanner::heuristicFunction(&ring[region], goal);
				if (temp < closest) {
					closest = temp;
					nextClosest = &ring[region];
				}
			}
			if (nextClosest) break;
		}*/
		
		*next = nullptr;
	}

	//Console::Write << "Next Tile: ";
	//if (parent[next]) Console::WriteLine << PrintTileInfo(next, GetGVal(next), GetRHS(next), parent[next]);
	//else Console::WriteLine << PrintTileInfoNull(next, GetGVal(next), GetRHS(next));
}

void MTDStarLite::ComputeInitialPath() {
	//Console::WriteLine << "Computing Cost Minimal Path";
	HexTile* cachedStart = *start;
	HexTile* cachedGoal = *goal;

	int iterations = 0;
	int maxIter = (int)PathPlanner::heuristicFunction(cachedStart, cachedGoal) * 15 + 30;
	bool hasPath = true;
	while (!open.empty() && (open.front().second < CalculateKey(cachedGoal) || GetRHSInit(cachedGoal) > GetGValInit(cachedGoal))) {
		if (iterations++ > maxIter) {
			Console::WriteLine << "Max iterations reached, NOT SEARCHING ANY FURTHER";
			hasPath = false;
			break;
		}

		HexTile* u = open.front().first;
		FloatPair kold = open.front().second;
		FloatPair knew = CalculateKey(u);

		if (kold < knew) {
			open.update(u, knew);
		}
		else if (GetGValInit(u) > GetRHSInit(u)) {
			knownTiles[u].g = knownTiles[u].rhs;
			open.remove(u);
			//if (parent[u]) Console::Write << PrintTileInfo(u, GetGVal(u), GetRHS(u), parent[u]);
			//else Console::Write << PrintTileInfoNull(u, GetGVal(u), GetRHS(u));
			//Console::WriteLine << " Tile was Over-consistent";
			//Console::WriteLine << " Updating susccessor neighbors...";

			ForEachSuccessor(u, [=](HexTile*const neighbor) {
				if (neighbor != cachedStart && (GetRHSInit(neighbor) > GetGValInit(u) + GetCostInit(neighbor))) {
					parent[neighbor] = u;
					//Console::WriteLine << PrintParentConnnection(neighbor, u);
					knownTiles[neighbor].rhs = PathPlanner::ClampInfinity(GetGValInit(u) + GetCostInit(neighbor));
					UpdateOpenList(neighbor);
				}
			});
		}
		else {
			knownTiles[u].g = grid->BlockWeight();
			//if (parent[u]) Console::Write << PrintTileInfo(u, GetGVal(u), GetRHS(u), parent[u]);
			//else Console::Write << PrintTileInfoNull(u, GetGVal(u), GetRHS(u));
			//Console::WriteLine << "Tile was Under-consistent";
			//Console::WriteLine << " Checking susccessor neighbors...";
			ForEachSuccessor(u, [=](HexTile*const neighbor) {
				if (neighbor != cachedStart && parent[neighbor] == u) {
					knownTiles[neighbor].rhs = GetMinimumFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGValInit(prime) + GetCostInit(neighbor); });

					if (PathPlanner::EpsilonIsEqual(GetRHSInit(neighbor), grid->BlockWeight())) {
						parent[neighbor] = nullptr;
						//Console::WriteLine << "Path through here is Blocked";
						//Console::WriteLine << PrintParentConnnectionNull(neighbor);
					}
					else {
						HexTile* minTile = GetMinimumTileFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGValInit(prime) + GetCostInit(neighbor); });
						//Console::WriteLine << "Finding new Path from here";
						parent[neighbor] = minTile; /*Console::WriteLine << PrintParentConnnection(neighbor, minTile);*/
													//parent[neighbor] = minTile;
					}
				}
				UpdateOpenList(neighbor);
			}, UnionSelf);
		}
	}
	//Console::WriteLine << "End Calculation";

	if (hasPath) {
		path.clear();
		//Console::WriteLine << "Begin building path";
		path.BuildPath(cachedStart, cachedGoal, parent); //might need to BuildPathReverse?
		*next = path.Next(cachedStart);
		//Console::WriteLine << "Path built";
		//for (int i = 0; i < path.size(); ++i) {
		//	HexTile* pathNode = path[i];
		//	if (parent[pathNode]) Console::WriteLine << PrintTileInfo(pathNode, GetGVal(pathNode), GetRHS(pathNode), parent[pathNode]);
		//	else Console::WriteLine << PrintTileInfoNull(pathNode, GetGVal(pathNode), GetRHS(pathNode));
		//}
	}
	else {
		/*float closest = grid->BlockWeight() + 1;
		float temp = 0;
		HexTile* nextClosest = nullptr;
		HexRegion ring;
		for (int range = (int)perceptionRange * 2; range > 0; --range) {
		ring = grid->Ring(start, range);
		for (int region = 0; region < ring.size(); ++region) {
		if (grid->IsBlocked(&ring[region])) continue;
		temp = PathPlanner::heuristicFunction(&ring[region], goal);
		if (temp < closest) {
		closest = temp;
		nextClosest = &ring[region];
		}
		}
		if (nextClosest) break;
		}*/

		*next = nullptr;
	}

	//Console::Write << "Next Tile: ";
	//if (parent[next]) Console::WriteLine << PrintTileInfo(next, GetGVal(next), GetRHS(next), parent[next]);
	//else Console::WriteLine << PrintTileInfoNull(next, GetGVal(next), GetRHS(next));
}

void MTDStarLite::OptimizedDelete() {
	//Console::WriteLine << "Optimized Delete";
	deleted.clear();

	parent[*start] = nullptr;
	//Console::WriteLine << PrintTileInfoNull(start, GetGVal(start), GetRHS(start));

	ForEachInSearchTreeButNotSubtreeRootedAt(*start, [=](HexTile*const s) {

		parent[s] = nullptr;
		//Console::WriteLine << PrintParentConnnectionNull(s);
		SetRHS(s, grid->BlockWeight());
		SetGVal(s, grid->BlockWeight());
		//Console::Write << PrintTileInfoNull(s, GetGVal(s), GetRHS(s));
		//Console::WriteLine << " Dropped parent";

		if (open.contains(s)) {
			open.remove(s);
		}

		deleted.push_back(s);
	});

	for (auto& del : deleted) {

		ForEachPredessor(del, [=](HexTile*const sPrime) {
			if (GetRHS(del) > PathPlanner::ClampInfinity(GetGVal(sPrime) + GetCost(del))) {
				knownTiles[del].rhs = PathPlanner::ClampInfinity(GetGVal(sPrime) + GetCost(del)); //can opt to only calc this once
				parent[del] = sPrime;
				//Console::Write << PrintTileInfo(del, GetGVal(del), GetRHS(del), sPrime);
				//Console::WriteLine << "Updated parent";
			}
		});

		if (GetRHS(del) < grid->BlockWeight()) {
			open.insert(del, CalculateKey(del));
		}
	}
	//Console::WriteLine << "End Optimized Delete";
}

MTDStarLite::MTDStarLite(HexGrid *const _grid, HexTile **const _start, HexTile **const _goal, HexTile **const _next, std::size_t _perception) :
	DStarCommon(_start, _goal, _next, _grid, _perception) {

	//this may not be neccessary
	//grid->ForEach([=](HexTile*const tile) {GetRHS(tile] = GetGVal(tile] = grid->BlockWeight(); /*parent[tile] = nullptr; */});

	HexTile* cachedStart = *_start;
	Console::WriteLine << "Initialize";
	parent[cachedStart] = nullptr;
	//Console::WriteLine << PrintParentConnnectionNull(start);
	SetRHS(cachedStart, 0);
	open.insert(cachedStart, CalculateKey(cachedStart));
	oldStart = *_start;
	oldGoal = *_goal;
	ComputeInitialPath();
}

MTDStarLite& MTDStarLite::operator=(MTDStarLite& _other) {
	path = _other.path;
	oldStart = _other.oldStart;
	oldGoal = _other.oldGoal;
	deleted.swap(_other.deleted);
	parent.swap(_other.parent);
	DStarCommon::operator=(_other);
	return *this;
}

void MTDStarLite::Update() {
	//Console::WriteLine << "Updating";
	//while(start != goal)
	
	//ComputeCostMinimalPath();
	//while(target not caught && on path from start to goal) {

	//if(isCaught) {
	//Console::WriteLine << "I've been caught!!!!!";
	//return;
	//}
	//if (!(*start)) *start = oldStart;
	HexTile* cachedStart = *start;
	HexTile* cachedGoal = *goal;

	//for all directed edges with cost change
	bool costChanged = false, goalOffPath = false;
	//Console::WriteLine << "StartTile (" << start->q << ", " << start->r << ")  GoalTile (" << goal->q << ", " << goal->r << ")";
	if (!path.find(cachedGoal)) {
		km += PathPlanner::heuristicFunction(oldGoal, cachedGoal);
		goalOffPath = true;
	}
	if (changedTiles.size()) {
		HexTile* u = cachedStart; //Detection start point

		for (HexTile& n : grid->Spiral(u, perceptionRange)) {
			HexTile* neighbor = grid->GetTileExact(n);
			//with changed edge costs
			if (!neighbor || !changedTiles.count(neighbor)) { continue; }

			costChanged = true;

			//Console::WriteLine << "Cost change detected at:";
			//if (parent.find(neighbor) != parent.end() && parent[neighbor]) Console::WriteLine << PrintTileInfo(neighbor, GetGVal(neighbor), GetRHS(neighbor), parent[neighbor]);
			//else Console::WriteLine << PrintTileInfoNull(neighbor, GetGVal(neighbor), GetRHS(neighbor));

			//Console::WriteLine << "Cost delta size was: " << changedTiles.size();
			float c_old = GetCost(neighbor);
			changedTiles.erase(neighbor);
			//Console::WriteLine << "Cost delta size is now: " << changedTiles.size();
			knownTiles[neighbor].cost = neighbor->weight;

			ForEachPredessor(neighbor, [=](HexTile* const pred) {
				//Console::Write << "Predecessor: ";
				//if (parent.find(pred) != parent.end() && parent[pred]) Console::WriteLine << PrintTileInfo(pred, GetGVal(pred), GetRHS(pred), parent[pred]);
				//else Console::WriteLine << PrintTileInfoNull(pred, GetGVal(pred), GetRHS(pred));

				if (neighbor != cachedStart) {
					if (c_old > GetCost(neighbor)) {
						if (GetRHS(neighbor) > GetGVal(pred) + GetCost(neighbor)) { //clamp infinity?
							//Console::WriteLine << "Edge Cost lowered";
							parent[neighbor] = pred;
							//Console::WriteLine << PrintParentConnnection(neighbor, u);
							knownTiles[neighbor].rhs = PathPlanner::ClampInfinity(GetGVal(pred) + GetCost(neighbor));
							UpdateOpenList(neighbor);
						}
					}
					else {
						if (parent[neighbor] == pred) {
							SetRHS(neighbor, GetMinimumFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGVal(prime) + GetCost(neighbor); }));

							if (PathPlanner::EpsilonIsEqual(GetRHS(neighbor), grid->BlockWeight())) {
								parent[neighbor] = nullptr;
								//Console::WriteLine << "Path to goal is blocked";
								//Console::WriteLine << PrintParentConnnectionNull(neighbor);
							}
							else {
								//Console::WriteLine << "Finding the new connection";
								HexTile* minTile = GetMinimumTileFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGVal(prime) + GetCost(neighbor); });
								//if (minTile) { parent[pred] = minTile; /*Console::WriteLine << PrintParentConnnection(neighbor, minTile);*/ }
								parent[pred] = minTile;
							}
							UpdateOpenList(neighbor);
						}
					}
				}
			});
		}
	}
	if (costChanged || (goalOffPath && oldStart != cachedStart)) {
		//shift map
		OptimizedDelete();
		//BasicDeletion(oldStart);
		oldStart = cachedStart;
	}
	ComputeCostMinimalPath();

	if (PathPlanner::EpsilonIsEqual(GetRHS(cachedGoal), grid->BlockWeight())) {
		//no path exists
		Console::WriteLine << "There's no PATH for MEE!!!!!";

		return;
	}
	//}
	//}
	//Console::WriteLine << "End Update";

	oldGoal = cachedGoal;
}

//void MTDStarLite::UpdateGoalReference(DirectX::XMFLOAT4X4* newgoal) {
//	goalReference = newgoal;
//}

#pragma endregion
