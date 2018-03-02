#include "PathPlanner.h"
#include "HexTileVector.h"
#include <queue>
#include "Heuristics.h"
#include "HexGrid.h"
#include "TraversalResult.h"
#include "Console.h"
#include <map>
#include <functional>
#include <iterator>

template<typename T, typename priority_t>
struct PriorityQueue {
	//we should profile and check if this is a bottle neck.
	//it uses a binary heap behind the scenes but may be worth it to switch to a deque if it is slow with large/complex paths
	typedef std::pair<priority_t, T> PQElement;
	std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

	inline bool empty() const { return elements.empty(); }

	inline void push(T item, priority_t priority) { elements.emplace(priority, item); }

	T top() {
		T top = elements.top().second;
		return top;
	}

	PQElement top_key() {
		PQElement top = elements.top();
		return top;
	}

	void pop() {
		elements.pop();
	}

	T pop_back() {
		T pop = elements.top().second;
		elements.pop();
		return pop;
	}
};

template<typename K, typename V>
struct BiDePriorityQueueMap {
	//typedef std::pair<K, V> pair;
	std::deque<std::pair<K, V>> q;

	static bool Predicate(std::pair<K, V> const& p1, std::pair<K, V> const& p2) {
		return p1.second > p2.second;
	}

	bool empty() const { return q.empty(); }

	void clear() { q.clear(); }

	std::size_t size() const { return q.size(); }

	void insert(K const& key, V const& value) {
		std::pair<K, V> node = std::make_pair(key, value);
		q.insert(std::upper_bound(q.begin(), q.end(), node, Predicate), node);
	}

	std::pair<K, V> front() const { return q.back(); }

	void pop() { q.pop_back(); }

	typename std::deque<std::pair<K, V>>::iterator find(K const& key) {
		return std::find_if(q.begin(), q.end(), [=](const std::pair<K, V>& element) { return element.first == key; });
	}
	
	typename std::deque<std::pair<K, V>>::iterator remove(K const& key) { return q.erase(find(key)); }

	bool contains(K const& key) { return find(key) != q.end(); }

	void update(K const& key, V const& value) { remove(key); insert(key, value); }
};

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

class DStarLite {
	typedef std::pair<float, float> pair_float;
	//typedef std::multimap<pair_float, HexTile*const, std::greater<pair_float>> priority_map;
	//priority_map U;

	BiDePriorityQueueMap<HexTile*, pair_float> U;

	float km;
	HexGrid *const grid = nullptr;
	HexTile * start = nullptr, *const goal = nullptr;
	HexTile* last = nullptr;
	HexTile* curNode = nullptr;

	HexTile** nextTileInPath = nullptr;

	CostMap cumulativeCost;
	CostMap rhs; //min of next cost

	float mostMinimum;

	typedef std::function<void(HexTile*const)> NeighborFunction;

	void ForEachPredessor(HexTile*const tile, NeighborFunction exec) {
		//pred = non blocked neighbors
		for(auto& element : tile->Neighbors()) {
			HexTile* neighbor = grid->GetTileExact(element);
			if(!neighbor || grid->IsBlocked(neighbor)) { continue; }
			exec(neighbor);
		}
	}

	void ForEachSuccessor(HexTile*const tile, NeighborFunction exec) {
		//succ = if you blocked, no res, else return all
		if(grid->IsBlocked(tile)) return;

		for(auto& element : tile->Neighbors()) {
			HexTile* neighbor = grid->GetTileExact(element);
			if(!neighbor) { continue; }
			exec(neighbor);
		}
	}

	std::pair<float, float> CalcKey(HexTile *const tile) {
		return {min(cumulativeCost[tile], rhs[tile]) + PathPlanner::heuristicFunction(start, tile) + km, min(cumulativeCost[tile],rhs[tile])};
	}

	void UpdateVertex(HexTile *const tile) {
		if(cumulativeCost[tile] != rhs[tile] && U.contains(tile)) {
			U.update(tile, CalcKey(tile));
		} else if(cumulativeCost[tile] != rhs[tile] && !U.contains(tile)) {
			U.insert(tile, CalcKey(tile));
		} else if(cumulativeCost[tile] == rhs[tile] && U.contains(tile)) {
			U.remove(tile);
		}
	}

	//void Replan() {
	//	path->clear();
	//
	//	curNode = start;
	//
	//	while(curNode != goal) {
	//		path->push_back(curNode);
	//
	//		mostMinimum = grid->BlockWeight();
	//		ForEachSuccessor(curNode, [=](HexTile*const sPrime) {
	//			float minposib = sPrime->weight + cumulativeCost[sPrime];
	//			if(minposib < mostMinimum) {
	//				mostMinimum = minposib;
	//				curNode = sPrime;
	//			}
	//		});
	//
	//		if(grid->IsBlocked(curNode)) {
	//			path->clear();
	//			return; //no path
	//		}
	//
	//	}
	//
	//	path->push_back(goal);
	//
	//}

	void ComputeShortestPath() {
		while(!U.empty() && (U.front().second < CalcKey(start) || rhs[start] > cumulativeCost[start])) {
			HexTile* u = U.front().first;
			pair_float kold = U.front().second;
			pair_float knew = CalcKey(u);
			if(kold < knew) {
				U.update(u, knew);
			} else if(cumulativeCost[u] > rhs[u]) {
				cumulativeCost[u] = rhs[u];
				U.remove(u);

				ForEachPredessor(u, [=](HexTile*const neighbor) {
					if(neighbor != goal) {
						rhs[neighbor] = min(rhs[neighbor], u->weight + cumulativeCost[u]);
					}
					UpdateVertex(neighbor);
				});
			} else {
				float gold = cumulativeCost[u];
				cumulativeCost[u] = grid->BlockWeight();

				ForEachPredessor(u, [=](HexTile*const neighbor) {
					if(rhs[neighbor] == u->weight + gold) {
						if(neighbor != goal) {
							mostMinimum = grid->BlockWeight();

							ForEachSuccessor(neighbor, [=](HexTile*const neighbor2) {
								float minposib = neighbor2->weight + cumulativeCost[neighbor2];
								mostMinimum = minposib < mostMinimum ? minposib : mostMinimum;
							});

							rhs[neighbor] = mostMinimum;
						}
					}
					UpdateVertex(neighbor);
				});
			}
		}
		//Replan();
	}

public:
	DStarLite & operator=(const DStarLite& other) {
		return *this;
	}

	DStarLite(HexGrid *const _grid, HexTile *const _start, HexTile *const _goal, HexTile** _nextTileInPath) :
		last(_start), grid(_grid), start(_start), goal(_goal), nextTileInPath(_nextTileInPath) {

		km = 0;

		*nextTileInPath = start;

		grid->ForEach([=](HexTile*const tile) {rhs[tile] = cumulativeCost[tile] = grid->BlockWeight(); });

		rhs[goal] = 0;

		U.insert(goal, std::make_pair(PathPlanner::heuristicFunction(start, goal), 0.0f));

		ComputeShortestPath();
	}

	void Update() {//ran on thread
	//	while(start != goal) {
		if(rhs[start] == grid->BlockWeight()) {
			//there is no known path
			Console::WriteLine << "There's no PATH for MEE!!!!!";
			return;
		}

		//find next node on path
		mostMinimum = grid->BlockWeight();
		ForEachSuccessor(start, [=](HexTile*const sPrime) {
			float minposib = sPrime->weight + cumulativeCost[sPrime];
			if(minposib < mostMinimum) {
				mostMinimum = minposib;
				start = sPrime;
			}
		});

		//move enemy to start
		*nextTileInPath = start;

		//scan graph for changed edge costs
		if(grid->GetCostDelta().size()) { //if any edge costs changed

			//for all directed edges
			for(auto& n : start->Neighbors()) {
				HexTile* neighbor = grid->GetTileExact(n);
				//with changed edge costs
				if(!neighbor || !grid->GetCostDelta().count(neighbor)) { continue; }
				km = km + PathPlanner::heuristicFunction(last, start);

				last = start;

				auto u = start; // ?????

				float c_old = grid->GetCostDelta()[neighbor];
				grid->GetCostDelta().erase(neighbor);

				if(c_old > neighbor->weight) {
					if(u != goal) {
						rhs[u] = min(rhs[u], neighbor->weight + cumulativeCost[neighbor]);
					}
				} else if(rhs[u] == c_old + cumulativeCost[neighbor]) {
					if(u != goal) {
						mostMinimum = grid->BlockWeight();

						ForEachSuccessor(u, [=](HexTile*const neighbor2) {
							float minposib = neighbor2->weight + cumulativeCost[neighbor2];
							mostMinimum = minposib < mostMinimum ? minposib : mostMinimum;
						});

						rhs[u] = mostMinimum;
					}
				}

				UpdateVertex(u);
			}

			ComputeShortestPath();
			//grid->GetCostDelta().clear();
		}
	}
	//}
};

std::vector<DStarLite> PathPlanner::dstarList;
std::size_t PathPlanner::dstars = 0;
std::size_t PathPlanner::DStarLiteSearch(HexTile *const start, HexTile *const goal, HexTile** nextTileInPath, HeuristicFunction Heuristic) {
	PathPlanner::SetHeuristic(Heuristic);

	auto ds = DStarLite(grid, start, goal, nextTileInPath);
	dstarList.insert(dstarList.begin() + dstars, ds);

	return dstars++;
}

void PathPlanner::UpdateDStarLite(std::size_t dstarId) {
	dstarList[dstarId].Update();
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
