#include "PathPlanner.h"
#include "Heuristics.h"
#include "HexGrid.h"
//#include "HexTileVector.h"
//#include "TraversalResult.h"
//#include <queue>
//#include <functional>
//#include <set>
#include "Console.h"
#include <map>
#include <iterator>
#include "DStar.h"
using namespace Common;

HeuristicFunction PathPlanner::heuristicFunction = nullptr;
HexGrid* PathPlanner::grid = nullptr;

void PathPlanner::SetHeuristic(HeuristicFunction heuristic) { heuristicFunction = heuristic; }

void PathPlanner::SetGrid(HexGrid* _grid) { grid = _grid; }

PathingAlgorithm PathPlanner::ChooseAlgorithm(TileType startType, TileType goalType) {
	if(!heuristicFunction) { SetHeuristic(Heuristics::ManhattanDistance); }

	switch(static_cast<int>(startType) ^ (static_cast<int>(goalType) << 1)) {
	default: case 3: //Static Static
		return PathingAlgorithm::AStar;
	case 5: //Static Dynamic
		return PathingAlgorithm::AStar;
	case 0: //Dynamic Static
		return PathingAlgorithm::DStarLite;
	case 6: //Dynamic Dynamic
		return PathingAlgorithm::MTDStarLite;
	}
}

HexPath PathPlanner::FindPath(HexTile*const start, HexTile*const goal, TileType startType, TileType goalType) {
	if(!grid) {
		Console::ErrorLine << "NO GRID! FREAK OUT!";
		return HexPath();
	}
	switch(ChooseAlgorithm(startType, goalType)) {
	case PathingAlgorithm::BreadthFirst: return BreadthFirstSearch(start, goal);
	case PathingAlgorithm::Dijkstra: return DijkstraSearch(start, goal);
	case PathingAlgorithm::AStar: return AStarSearch(start, goal, heuristicFunction);
		//case PathingAlgorithm::DStarLite: return DStarLiteSearch(start, goal, heuristicFunction);
	default: return HexPath();
	}
}

HexPath PathPlanner::FindPath(const DirectX::XMFLOAT2 start, const DirectX::XMFLOAT2 goal, TileType startType, TileType goalType) {
	//HexTile* s = GetTileExact((int) start.x, (int) start.y);
	//HexTile* e = GetTileExact((int) goal.x, (int) goal.y);

	HexTile* s = grid->PointToTile(start);
	HexTile* e = grid->PointToTile(goal);

	if(s && e) {
		return FindPath(s, e, startType, goalType);
	}
	return HexPath();
}

template<PathingAlgorithm pathingAlgorithm>
inline typename std::enable_if<pathingAlgorithm == PathingAlgorithm::BreadthFirst || pathingAlgorithm == PathingAlgorithm::Dijkstra, TraversalResult>::type
PathPlanner::Traverse(HexTile * const start, std::size_t constraint, std::size_t maxSteps) {
	maxSteps = maxSteps ? maxSteps : constraint;
	return pathingAlgorithm == PathingAlgorithm::BreadthFirst ? BreadthFirstTraverse(start, constraint, maxSteps) : DijkstraTraverse(start, constraint, maxSteps);
}

TraversalResult PathPlanner::BreadthFirstTraverse(HexTile *const tile, size_t steps, size_t maxMovement) {
	VisitedMap visited;
	visited[tile] = nullptr;

	CostMap stepCost;
	stepCost[tile] = 0;

	std::vector<HexPath> reachableTiles = {};
	reachableTiles.resize(maxMovement);
	reachableTiles[0].push_back(tile);

	for(size_t i = 0; i < maxMovement - 1 && reachableTiles[i].size(); ++i) {
		for(size_t j = 0; j < reachableTiles[i].size(); ++j) {
			const HexTile* _this = reachableTiles[i][j];
			HexTile* _this_ = const_cast<HexTile*>(_this);

			for(auto& n : _this_->Neighbors()) {
				HexTile* neighbor = grid->GetTileExact(n);
				if(!neighbor || grid->IsBlocked(neighbor)) { continue; }

				bool notContain = !stepCost.count(neighbor);
				bool withinReach = tile->DistanceFrom(neighbor) <= (int) steps;

				if(notContain && withinReach) {
					stepCost[neighbor] = float(i + 1);
					reachableTiles[i + 1].push_back(neighbor);
					visited[neighbor] = _this_;
				}
			}
		}
	}

	TraversalResult res;
	res.visitedMap = visited;
	res.costMap = stepCost;
	res.reachableTiles = reachableTiles;

	return res;
}

TraversalResult PathPlanner::DijkstraTraverse(HexTile *const tile, size_t cost, size_t maxMovement) {
	PriorityQueue<HexPath, float> Q;
	HexPath p; p.push_back(tile);
	Q.push(p, 0);

	VisitedMap visited;
	visited[tile] = tile;

	CostMap weightCost;
	weightCost[tile] = 0;

	std::vector<HexPath> reachableTiles = {};
	reachableTiles.resize(maxMovement);

	for(size_t i = 0; i < maxMovement - 1 && !Q.empty(); ++i) {
		while(!Q.empty()) {
			HexPath current = Q.pop_back();
			reachableTiles.push_back(current);
			HexPath build;
			for(size_t j = 0; j < current.size(); ++j) {
				for(auto& n : current[j]->Neighbors()) {
					HexTile* neighbor = grid->GetTileExact(n);
					if(!neighbor || grid->IsBlocked(neighbor)) { continue; }

					float new_cost = weightCost[current[j]] + neighbor->weight;

					if(!weightCost.count(neighbor) || new_cost < weightCost[neighbor]) {
						if(new_cost > cost) continue;
						build.push_back(neighbor);
						weightCost[neighbor] = new_cost;
						Q.push(build, new_cost);
						visited[neighbor] = current[j];
					}
				}
			}
		}
	}

	TraversalResult res;
	res.visitedMap = visited;
	res.costMap = weightCost;
	res.reachableTiles = reachableTiles;

	return res;
}

HexPath PathPlanner::BreadthFirstSearch(HexTile *const start, HexTile *const goal) {
	std::queue<HexTile*> Q;
	VisitedMap visited;

	Q.push(start);
	visited[start] = nullptr;

	while(!Q.empty()) {
		HexTile* current = Q.front();
		Q.pop();

		if(current == goal) { break; }

		for(auto& neighbor : current->Neighbors()) {
			if(!visited.count(&neighbor)) {
				Q.push(&neighbor);
				visited[&neighbor] = current;
			}
		}
	}

	HexPath path;
	path.BuildPath(start, goal, visited);

	return path;
}

HexPath PathPlanner::DijkstraSearch(HexTile *const start, HexTile *const goal) {
	if(grid->IsBlocked(goal)) return HexPath();

	PriorityQueue<HexTile*, float> Q;
	VisitedMap visited;
	CostMap cumulativeCost;

	Q.push(start, 0);
	visited[start] = start;
	cumulativeCost[start] = 0;

	while(!Q.empty()) {
		HexTile* current = Q.pop_back();

		if(current == goal) { break; }

		for(auto& _n : current->Neighbors()) {
			HexTile* neighbor = grid->GetTileExact(_n);
			if(!neighbor || grid->IsBlocked(neighbor)) { continue; }
			//neighbor->DrawT(layout, {0,1,0});

			float new_cost = cumulativeCost[current] + neighbor->weight;
			if(!cumulativeCost.count(neighbor) || new_cost < cumulativeCost[neighbor]) {
				cumulativeCost[neighbor] = new_cost;
				visited[neighbor] = current;
				Q.push(neighbor, new_cost);
			}
		}
	}

	HexPath path;
	path.BuildPath(start, goal, visited);

	return path;
}

HexPath PathPlanner::AStarSearch(HexTile *const start, HexTile *const goal, HeuristicFunction Heuristic) {
	if(grid->IsBlocked(goal)) return HexPath();

	PriorityQueue<HexTile*, float> Q;
	//we could possibly not even use a priority queue here if we hash the tiles into buckets
	//say we know in advance what our weight range will be, we can make each of those a bucket and not need
	//to do any sorting in the first place

	VisitedMap visited;
	CostMap cumulativeCost;

	Q.push(start, 0);
	visited[start] = start;
	cumulativeCost[start] = 0;

	while(!Q.empty()) {
		HexTile* current = Q.pop_back();

		if(current == goal) { break; }

		for(auto& _n : current->Neighbors()) { //the call to Neighbors() can be optimized if instead I preallocate space to store the neighbors and pass it in to be filled out
			HexTile* neighbor = grid->GetTileExact(_n);
			if(!neighbor || grid->IsBlocked(neighbor)) { continue; }
			//neighbor->DrawX(layout, {0,1,0});

			float new_cost = cumulativeCost[current] + neighbor->weight;
			//you may have noticed I don't check for a node in the Q that has a high cost
			//This can result in duplicated elements in the Q, but the speed benefit is worth it
			//It has the possiblity to revisit some locations more than neccessary, but seldom happens
			//the priority Q doesnt need to support decrease-key (like we did in AI class) which makes it faster
			if(cumulativeCost.find(neighbor) == cumulativeCost.end() || new_cost < cumulativeCost[neighbor]) {
				cumulativeCost[neighbor] = new_cost;
				float priority = new_cost + Heuristic(neighbor, goal);
				Q.push(neighbor, priority);
				visited[neighbor] = current;
			}
		}
	}

	HexPath path;
	path.BuildPath(start, goal, visited);

	return path;
}

std::unordered_map<std::size_t, DStarLite> PathPlanner::dstarList;
std::size_t PathPlanner::dstarIndices = 0;
std::unordered_map<std::size_t, MTDStarLite> PathPlanner::mtdstarList;
std::size_t PathPlanner::mtdstarIndices = 0;

//std::vector<DStarLite> PathPlanner::dstarList; //Will this work properly?
//std::size_t PathPlanner::dstars = 0;
std::size_t PathPlanner::DStarLiteSearch(HexTile *const start, HexTile *const goal, HexTile** nextTileInPath, HeuristicFunction Heuristic) {
	PathPlanner::SetHeuristic(Heuristic);

	dstarList[dstarIndices] = DStarLite(grid, start, goal, nextTileInPath);

	return dstarIndices++;
}

//std::vector<MTDStarLite> PathPlanner::mtdstarList; //Will this work properly?
//std::size_t PathPlanner::mtdstars = 0;
std::size_t PathPlanner::MTDStarLiteSearch(DirectX::XMFLOAT4X4* startRef, DirectX::XMFLOAT4X4* goalRef, HeuristicFunction Heuristic) {
	PathPlanner::SetHeuristic(Heuristic);

	//auto ds = MTDStarLite(grid, startRef, goalRef);
	mtdstarList[mtdstarIndices] = MTDStarLite(grid, startRef, goalRef); 

	return mtdstarIndices++;
}

void PathPlanner::UpdateDStar(std::size_t dstarId) {
	dstarList[dstarId].Update();
}

void PathPlanner::UpdateMTDStar(std::size_t mtdstarId) {
	mtdstarList[mtdstarId].Update();
}

HexTile* PathPlanner::GetDStarNextTile(std::size_t dstarId) {
	return dstarList[dstarId].GetNextTileInPath();
}

HexTile* PathPlanner::GetMTDStarNextTile(std::size_t mtdstarId) {
	return mtdstarList[mtdstarId].GetNextTileInPath();
}

void PathPlanner::UpdateMTDSLTargetReference(std::size_t mtdstarId, DirectX::XMFLOAT4X4* goalRef) {
	//MTDStarLite* ds = dynamic_cast<MTDStarLite*>(&dstarList[mtdstarId]); //Don't like this!
	mtdstarList[mtdstarId].UpdateGoalReference(goalRef);
}

bool PathPlanner::RemoveDStar(std::size_t dstarId) {
	if (dstarList.count(dstarId)) {
		dstarList.erase(dstarId);
		return true;
	}
	return false;
}

bool PathPlanner::RemoveMTDStar(std::size_t mtdstarId) {
	if (dstarList.count(mtdstarId)) {
		dstarList.erase(mtdstarId);
		return true;
	}
	return false;
}

//void PathPlanner::UpdateMTDStarLite(std::size_t mtdstarId) {
//	dstarList[mtdstarId].Update();
//}
//
//HexTile* PathPlanner::GetMTDStarNextTile(std::size_t mtdstarId) {
//	return mtdstarList[mtdstarId].GetNextTileInPath();
//}

void PathPlanner::CostChangeNotice(HexTile* tile) {
	for (auto &ds : dstarList) {
		ds.second.changedTiles.insert(tile);
	}
	for (auto &mtds : mtdstarList) {
		mtds.second.changedTiles.insert(tile);
	}
	//for (int i = 0; i < dstarList.size(); ++i) {
	//	dstarList[i].changedTiles.insert(tile);
	//}
	//for (int i = 0; i < mtdstarList.size(); ++i) {
	//	mtdstarList[i].changedTiles.insert(tile);
	//}

}

HexPath PathPlanner::CalculatePathWithinXSteps(HexTile *const start, HexTile *const goal, size_t steps) {
	if(grid->IsBlocked(goal)) {
		//	Console::WriteLine << "Goal is blocked!";
		return HexPath();
	}

	TraversalResult search = BreadthFirstTraverse(goal, steps, steps);

	//for(auto& vec : search.reachableTiles) {
	//	vec.Color(&layout, {1,0,0}, 0, ColorType::__X);
	//}

	//HexRegion shadow;
	//for(auto& t : search.costMap) {
	//	if(t.second > (int) steps) { shadow.push_back(*t.first); }
	//}
	//shadow.Color(&layout, {0,0,1}, 0, ColorType::__T);

	int distance = start->DistanceFrom(goal);
	//Console::WriteLine << "Distance is " << distance << "  steps is " << steps;
	HexPath path;

	if(distance >= (int) steps) {
		//Console::WriteLine << "Path is too far!";
	} else {
		path.BuildPathReverse(start, goal, search.visitedMap);

		//if(path.size() == 2 && distance > 1) {
		//	Console::WriteLine << "Path is not reachable or too far!";
		//} else {
		//Console::WriteLine << "Path has: " << path.size() << " elements.";
		//path.Color(&layout, {0,1,0}, 0.0f, ColorType::__mX);
		//}
	}

	return path;
}

HexPath PathPlanner::CalculatePathWithinXCost(HexTile *const start, HexTile *const goal, size_t cost) {
	if(grid->IsBlocked(goal)) {
		return HexPath();
	}

	TraversalResult search = DijkstraTraverse(goal, cost, cost);

	//for(auto& t : map) {
	//	t->DrawmX(layout, {t->weight / 4.0f, 0, 1}, 0);  //weights are 1-4 in fill
	//}

	//for(auto& vec : search.reachableTiles) {
	//	vec.Color(&layout, {0,1,1}, 0, ColorType::__X);
	//}

	HexPath path;

	path.BuildPathReverse(start, goal, search.visitedMap);

	//Console::WriteLine << "Path has: " << path.size() << " elements.";
	//path.Color(&layout, {0,1,0}, 0.0f, ColorType::__mX);

	return path;
}

float PathPlanner::ClampInfinity(float num) {
	return num > grid->BlockWeight() ? grid->BlockWeight() : num;
}

bool PathPlanner::EpsilonIsEqual(float num1, float num2) {
	return fabsf(num1 - num2) < FLT_EPSILON;
}
