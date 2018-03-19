#include "DStar.h"
#include "PathPlanner.h"
#include "HexGrid.h"
#include "HexagonTile.h"
#include "HexTileVector.h"
#include "Console.h"

#define PrintParentConnnection(t, p) "Tile (" << t->q << ", " << t->r << ") -> ParentTile (" << p->q << ", " << p->r << ")"
#define PrintParentConnnectionNull(t) "Tile (" << t->q << ", " << t->r << ") -> ParentTile (null)"
#define PrintTileInfo(t, g, rhs, p) "Tile (" << t->q << ", " << t->r << "), c(" << t->weight << "), g(" << g << "), rhs(" << rhs << ") ParentTile (" << p->q << ", " << p->r << ")"
#define PrintTileInfoNull(t, g, rhs) "Tile (" << t->q << ", " << t->r << "), c(" << t->weight << "), g(" << g << "), rhs(" << rhs << ") ParentTile (NULL)"


#pragma region DStarCommon

DStarCommon::DStarCommon(HexTile *const _start, HexTile *const _goal, HexGrid *const _grid) : start(_start), goal(_goal), grid(_grid), km(0.0f) {}

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
	return { PathPlanner::ClampInfinity(min(GetGVal(tile), GetRHS(tile)) + PathPlanner::heuristicFunction(tile, goal) + km), min(GetGVal(tile), GetRHS(tile)) };
}

float DStarCommon::GetGVal(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), tile->weight };
	}
	return knownTiles[tile].g;
}

float DStarCommon::GetRHS(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), tile->weight };
	}
	return knownTiles[tile].rhs;
}

float DStarCommon::GetCost(HexTile* tile) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), tile->weight };
	}
	return knownTiles[tile].cost;
}

void DStarCommon::SetGVal(HexTile* tile, float value) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), tile->weight };
	}
	knownTiles[tile].g = value;
}

void DStarCommon::SetRHS(HexTile* tile, float value) {
	if (knownTiles.count(tile) == 0) {
		knownTiles[tile] = TileInfo{ grid->BlockWeight(), grid->BlockWeight(), tile->weight };
	}
	knownTiles[tile].rhs = value;
}

void DStarCommon::UpdateOpenList(HexTile *const tile) {
	//if (parent[tile]) Console::Write << PrintTileInfo(tile, GetGVal(tile], GetRHS(tile], parent[tile]);
	//else Console::Write << PrintTileInfoNull(tile, GetGVal(tile], GetRHS(tile]);
	if (!PathPlanner::EpsilonIsEqual(GetGVal(tile), GetRHS(tile))) {
		Console::WriteLine << " Tile was Inconsistent";
		if (open.contains(tile)) {
			open.update(tile, CalculateKey(tile));
		}
		else {
			open.insert(tile, CalculateKey(tile));
		}
	}
	else if (PathPlanner::EpsilonIsEqual(GetGVal(tile), GetRHS(tile)) && open.contains(tile)) {
		Console::WriteLine << " Tile was Consistent";
		open.remove(tile);
	}
}

#pragma endregion

#pragma region DStarLite

void DStarLite::ComputeShortestPath() {
	while (!open.empty() && (open.front().second < CalculateKey(start) || GetRHS(start) > GetGVal(start))) {
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
				if (neighbor != goal) {
					knownTiles[neighbor].rhs = PathPlanner::ClampInfinity((min(GetRHS(neighbor), u->weight + GetGVal(u))));
				}
				UpdateOpenList(neighbor);
			});

		}
		else {
			float gold = GetGVal(u);
			knownTiles[u].g = grid->BlockWeight();

			ForEachPredessor(u, [=](HexTile*const neighbor) {
				if (PathPlanner::EpsilonIsEqual(GetRHS(neighbor), PathPlanner::ClampInfinity(u->weight + gold))) {
					if (neighbor != goal) {
						knownTiles[neighbor].rhs = GetMinimumFrom(SuccessorsOf, neighbor, [=](HexTile*const prime) {return prime->weight + GetGVal(prime); });
					}
				}
				UpdateOpenList(neighbor);
			}, UnionSelf);

		}
	}
	//Replan();
}

DStarLite::DStarLite(HexGrid *const _grid, HexTile *const _start, HexTile *const _goal, HexTile** _nextTileInPath) :
	last(_start), nextTileInPath(_nextTileInPath), DStarCommon(_start, _goal, _grid) {

	*nextTileInPath = start;

	//this may not be neccessary
	//grid->ForEach([=](HexTile*const tile) {GetRHS(tile] = GetGVal(tile] = grid->BlockWeight(); });

	SetRHS(goal, 0.0f);

	open.insert(goal, std::make_pair(PathPlanner::heuristicFunction(start, goal), 0.0f));

	ComputeShortestPath();
}

void DStarLite::Update() {//ran on thread
			   //	while(start != goal) {
	if (GetRHS(start) == grid->BlockWeight()) {
		//there is no known path
		Console::WriteLine << "There's no PATH for MEE!!!!!";
		return;
	}

	//find next node on path
	HexTile* nextNode = GetMinimumTileFrom(SuccessorsOf, start, [=](HexTile*const sPrime) { return sPrime->weight + GetGVal(sPrime); });
	if (nextNode) { start = nextNode; }

	//move enemy to start
	*nextTileInPath = start;

	//scan graph for changed edge costs
	if (changedTiles.size()) { //if any edge costs changed
		//HexRegion range = grid->GetTilesNStepsAway(start, 3);
		HexRegion range = grid->Spiral(start, perceptionRange);
		//for all directed edges
		for (HexTile& n : range /* grid->GetTilesNStepsAway(start, 3)*/) {
			HexTile* neighbor = grid->GetTileExact(n);
			//with changed edge costs
			if (changedTiles.count(neighbor)) { continue; }
			km = km + PathPlanner::heuristicFunction(last, start);
			last = start;

			float c_old = GetCost(neighbor);
			//This WILL NOT WORK when there are multiple enemies trying to detect change on neighbor
			changedTiles.erase(neighbor);
			knownTiles[neighbor].cost = neighbor->weight;

			ForEachPredessor(neighbor, [=](HexTile* const pred) {
				if (c_old > neighbor->weight) {
					if (pred != goal) {
						knownTiles[pred].rhs = PathPlanner::ClampInfinity(min(GetRHS(pred), neighbor->weight + GetGVal(neighbor)));
					}
				}
				else if (PathPlanner::EpsilonIsEqual(GetRHS(pred), PathPlanner::ClampInfinity(c_old + GetGVal(neighbor)))) {
					if (pred != goal) {
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
		//traveled.clear();
		while (nextParent) {
			if (nextParent == tile) break;
			if (traveled.count(tile) > 0) break;
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
	parent[start] = nullptr;
	//Console::WriteLine << PrintParentConnnectionNull(start);
	GetRHS(oldStart] = GetMinimumFrom(PredessorsOf, oldStart, [=](HexTile*const prime) { return GetGVal(prime] + oldStart->weight; });
	if (PathPlanner::EpsilonIsEqual(GetRHS(oldStart], grid->BlockWeight())) {
		parent[oldStart] = nullptr;
		//Console::WriteLine << PrintParentConnnectionNull(oldStart);
	}
	else {
		parent[oldStart] = GetMinimumTileFrom(PredessorsOf, oldStart, [=](HexTile*const prime) { return GetGVal(prime] + oldStart->weight; });
		//Console::WriteLine << PrintParentConnnection(oldStart, parent[oldStart]);
	}
	UpdateOpenList(oldStart);
}

void MTDStarLite::ComputeCostMinimalPath() {
	Console::WriteLine << "Computing Cost Minimal Path";
	while (!open.empty() && (open.front().second < CalculateKey(goal) || GetRHS(goal] > GetGVal(goal])) {
		HexTile* u = open.front().first;
		FloatPair kold = open.front().second;
		FloatPair knew = CalculateKey(u);

		if (kold < knew) {
			open.update(u, knew);
		}
		else if (GetGVal(u] > GetRHS(u]) {
			GetGVal(u] = GetRHS(u];
			open.remove(u);
			if (parent[u]) Console::Write << PrintTileInfo(u, GetGVal(u], GetRHS(u], parent[u]);
			else Console::Write << PrintTileInfoNull(u, GetGVal(u], GetRHS(u]);
			Console::WriteLine << " Tile was Over-consistent";
			Console::WriteLine << " Updating susccessor neighbors...";

			ForEachSuccessor(u, [=](HexTile*const neighbor) {
				if (neighbor != start && (GetRHS(neighbor] > GetGVal(u] + GetCost(neighbor))) {
					parent[neighbor] = u;
					//Console::WriteLine << PrintParentConnnection(neighbor, u);
					GetRHS(neighbor] = PathPlanner::ClampInfinity(GetGVal(u] + GetCost(neighbor));
					UpdateOpenList(neighbor);
				}
			});
		}
		else {
			GetGVal(u] = grid->BlockWeight();
			if (parent[u]) Console::Write << PrintTileInfo(u, GetGVal(u], GetRHS(u], parent[u]);
			else Console::Write << PrintTileInfoNull(u, GetGVal(u], GetRHS(u]);
			Console::WriteLine << "Tile was Under-consistent";
			Console::WriteLine << " Checking susccessor neighbors...";
			ForEachSuccessor(u, [=](HexTile*const neighbor) {
				if (neighbor != start && parent[neighbor] == u) {
					GetRHS(neighbor] = GetMinimumFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGVal(prime] + GetCost(neighbor); });

					if (PathPlanner::EpsilonIsEqual(GetRHS(neighbor], grid->BlockWeight())) {
						parent[neighbor] = nullptr;
						Console::WriteLine << "Path through here is Blocked";
						//Console::WriteLine << PrintParentConnnectionNull(neighbor);
					}
					else {
						HexTile* minTile = GetMinimumTileFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGVal(prime] + GetCost(neighbor); });
						Console::WriteLine << "Finding new Path from here";
						parent[neighbor] = minTile; /*Console::WriteLine << PrintParentConnnection(neighbor, minTile);*/
													//parent[neighbor] = minTile;
					}
				}
				UpdateOpenList(neighbor);
			}, UnionSelf);
		}
	}
	Console::WriteLine << "End Calculation";

	path->clear();
	Console::WriteLine << "Begin building path";
	(*path).BuildPath(start, goal, parent); //might need to BuildPathReverse?
	Console::WriteLine << "Path built: ";
	for (int i = 0; i < path->size(); ++i) {
		HexTile* pathNode = (*path)[i];
		if (parent[pathNode]) Console::WriteLine << PrintTileInfo(pathNode, GetGVal(pathNode], GetRHS(pathNode], parent[pathNode]);
		else Console::WriteLine << PrintTileInfoNull(pathNode, GetGVal(pathNode], GetRHS(pathNode]);
	}
}

void MTDStarLite::OptimizedDelete() {
	Console::WriteLine << "Optimized Delete";
	deleted.clear();

	parent[start] = nullptr;
	Console::WriteLine << PrintTileInfoNull(start, GetGVal(start], GetRHS(start]);

	ForEachInSearchTreeButNotSubtreeRootedAt(start, [=](HexTile*const s) {

		parent[s] = nullptr;
		//Console::WriteLine << PrintParentConnnectionNull(s);
		GetRHS(s] = GetGVal(s] = grid->BlockWeight();
		Console::Write << PrintTileInfoNull(s, GetGVal(s], GetRHS(s]);
		Console::WriteLine << " Dropped parent";

		if (open.contains(s)) {
			open.remove(s);
		}

		deleted.push_back(s);
	});

	for (auto& del : deleted) {

		ForEachPredessor(del, [=](HexTile*const sPrime) {
			if (GetRHS(del] > PathPlanner::ClampInfinity(GetGVal(sPrime] + GetCost(del))) {
				GetRHS(del] = PathPlanner::ClampInfinity(GetGVal(sPrime] + GetCost(del)); //can opt to only calc this once
				parent[del] = sPrime;
				Console::Write << PrintTileInfo(del, GetGVal(del], GetRHS(del], sPrime);
				Console::WriteLine << "Updated parent";
			}
		});

		if (GetRHS(del] < grid->BlockWeight()) {
			open.insert(del, CalculateKey(del));
		}
	}
	Console::WriteLine << "End Optimized Delete";
}

MTDStarLite::MTDStarLite(HexGrid *const _grid, DirectX::XMFLOAT4X4* _startRef, DirectX::XMFLOAT4X4* _goalRef, HexPath* _path) :
	path(_path), goalReference(_goalRef), startReference(_startRef), DStarCommon(nullptr, nullptr, _grid) {

	//this may not be neccessary
	grid->ForEach([=](HexTile*const tile) {GetRHS(tile] = GetGVal(tile] = grid->BlockWeight(); /*parent[tile] = nullptr; */});

	goal = grid->PointToTile(DirectX::XMFLOAT2(goalReference->_41, goalReference->_43));
	start = grid->PointToTile(DirectX::XMFLOAT2(startReference->_41, startReference->_43));

	//Console::WriteLine << "Initialize";
	parent[start] = nullptr;
	//Console::WriteLine << PrintParentConnnectionNull(start);
	GetRHS(start] = 0;
	open.insert(start, CalculateKey(start));
	ComputeCostMinimalPath();
}

void MTDStarLite::Update() {
	Console::WriteLine << "Updating";
	//while(start != goal)
	HexTile* oldStart = start;
	HexTile* oldGoal = goal;
	//ComputeCostMinimalPath();

	if (PathPlanner::EpsilonIsEqual(GetRHS(goal], grid->BlockWeight())) {
		//no path exists
		Console::WriteLine << "There's no PATH for MEE!!!!!";
		return;
	}

	//while(target not caught && on path from start to goal) {

	//if(isCaught) {
	//Console::WriteLine << "I've been caught!!!!!";
	//return;
	//}

	//for all directed edges with cost change
	goal = grid->PointToTile(DirectX::XMFLOAT2(goalReference->_41, goalReference->_43));
	start = grid->PointToTile(DirectX::XMFLOAT2(startReference->_41, startReference->_43));
	bool costChanged = false, goalOffPath = false;
	Console::WriteLine << "StartTile (" << start->q << ", " << start->r << ")  GoalTile (" << goal->q << ", " << goal->r << ")";
	if (!path->find(goal)) {
		km += PathPlanner::heuristicFunction(oldGoal, goal);
		goalOffPath = true;
	}
	if (costChange.size()) {
		HexTile* u = start; //Detection start point

		for (HexTile& n : u->Neighbors()) {
			HexTile* neighbor = grid->GetTileExact(n);
			//with changed edge costs
			if (!neighbor || !costChange.count(neighbor)) { continue; }

			costChanged = true;

			Console::WriteLine << "Cost change detected at:";
			if (parent.find(neighbor) != parent.end() && parent[neighbor]) Console::WriteLine << PrintTileInfo(neighbor, GetGVal(neighbor], GetRHS(neighbor], parent[neighbor]);
			else Console::WriteLine << PrintTileInfoNull(neighbor, GetGVal(neighbor], GetRHS(neighbor]);

			Console::WriteLine << "Cost delta size was: " << costChange.size();
			float c_old = costChange[neighbor];
			costChange.erase(neighbor);
			Console::WriteLine << "Cost delta size is now: " << costChange.size();

			ForEachPredessor(neighbor, [=](HexTile* const pred) {
				Console::Write << "Predecessor: ";
				if (parent.find(pred) != parent.end() && parent[pred]) Console::WriteLine << PrintTileInfo(pred, GetGVal(pred], GetRHS(pred], parent[pred]);
				else Console::WriteLine << PrintTileInfoNull(pred, GetGVal(pred], GetRHS(pred]);

				if (neighbor != start) {
					if (c_old > neighbor->weight) {
						if (GetRHS(neighbor] > GetGVal(pred] + neighbor->weight) { //clamp infinity?
							Console::WriteLine << "Edge Cost lowered";
							parent[neighbor] = pred;
							//Console::WriteLine << PrintParentConnnection(neighbor, u);
							GetRHS(neighbor] = PathPlanner::ClampInfinity(GetGVal(pred] + neighbor->weight);
							UpdateOpenList(neighbor);
						}
					}
					else {
						if (parent[neighbor] == pred) {
							GetRHS(neighbor] = GetMinimumFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGVal(prime] + neighbor->weight; });

							if (PathPlanner::EpsilonIsEqual(GetRHS(neighbor], grid->BlockWeight())) {
								parent[neighbor] = nullptr;
								Console::WriteLine << "Path to goal is blocked";
								//Console::WriteLine << PrintParentConnnectionNull(neighbor);
							}
							else {
								Console::WriteLine << "Finding the new connection";
								HexTile* minTile = GetMinimumTileFrom(PredessorsOf, neighbor, [=](HexTile*const prime) {return GetGVal(prime] + neighbor->weight; });
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
	if (costChanged || (goalOffPath && oldStart != start)) {
		//shift map
		OptimizedDelete();
		//BasicDeletion(oldStart);
	}
	ComputeCostMinimalPath();
	//}
	//}
	Console::WriteLine << "End Update";

}

//void UpdateGoal(HexTile* newgoal) {
//	goal = newgoal;
//}

#pragma endregion