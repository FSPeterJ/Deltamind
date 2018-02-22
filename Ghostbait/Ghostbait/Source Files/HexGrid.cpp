#include "HexGrid.h"
#include "Console.h"
#include "DebugRenderer.h"
#include "HexagonTile.h"
#include <chrono>
#include <queue>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "HexTileVector.h"
#include <random>

const float HexGrid::Blocked = float(0xDEAD);

namespace std {
	size_t hash<HexTile*>::operator()(const argument_type& data) const noexcept {
		std::hash<int> int_hash;
		size_t hq = int_hash(data->q);
		size_t hr = int_hash(data->r);
		//uses the magic number from boost::hash_combine.
		//it's essentially a number with an equal likelyhood of 1s and 0s to give 32 'random' bits
		//also it's the inverse of the golden ratio
		//the idea is, for each tile, consecutive values will be far apart, thus ensuring an even spread, even for small ranges
		return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
	}
}

bool EqualComparator::operator()(const HexTile* lhs, const HexTile* rhs) const { return *lhs == *rhs; }
bool EqualComparator::operator()(const HexTile& lhs, const HexTile& rhs) const { return lhs == rhs; }

bool operator<(const HexPath&p, const HexPath&p2) { return p.cost() <= p2.cost(); } //in case of a cost tie, std::pair will compare the second element so this is needed to satisfy it


template<typename T, typename priority_t>
struct PriorityQueue {
	//we should profile and check if this is a bottle neck.
	//it uses a binary heap behind the scenes but may be worth it to switch to a deque if it is slow with large/complex paths
	typedef std::pair<priority_t, T> PQElement;
	std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

	inline bool empty() const { return elements.empty(); }

	inline void push(T item, priority_t priority) { elements.emplace(priority, item); }

	T pop_back() {
		T best_item = elements.top().second;
		elements.pop();
		return best_item;
	}
};

HexGrid::HexGrid(float _radius, const HexagonalGridLayout& _layout) : map_radius(_radius), layout(_layout) {}

HexTile* HexGrid::PointToTile(const DirectX::XMFLOAT2& p) {
	return GetTileExact(HexTile::Round(HexagonalGridLayout::PointToHexagonTile(p, layout)));
}

DirectX::XMFLOAT2 HexGrid::TileToPoint(HexTile * tile) {
	return tile->Center(layout);
}

bool HexGrid::SetWeight(const DirectX::XMFLOAT2& tilePosition, float weight) {
	HexTile* t = PointToTile(tilePosition);
	return SetWeight(t, weight);
}

bool HexGrid::SetWeight(HexTile *const tile, float weight) {
	if(tile) {
		tile->weight = weight;
		return true;
	}
	return false;
}

bool HexGrid::AddObstacle(const DirectX::XMFLOAT2& obstaclePosition) {
	HexTile* t = PointToTile(obstaclePosition);
	if(t) {
		t->weight = Blocked;
		blocked.push_back(*t);
		return true;
	}
	return false;
}

bool HexGrid::RemoveObstacle(const DirectX::XMFLOAT2& obstaclePosition) {
	HexTile* t = PointToTile(obstaclePosition);
	if(t) {
		t->weight = 1;
		blocked.remove(*t);
		return true;
	}
	return false;
}

bool HexGrid::Snap(const DirectX::XMFLOAT2& p, OUT DirectX::XMFLOAT2& snapPoint) {
	HexTile* snapTile = PointToTile(p);
	if(snapTile) {
		snapPoint = snapTile->Center(layout);
		return true;
	}
	return false;
}

HexPath HexGrid::CalculatePathWithinXSteps(HexTile *const start, HexTile *const goal, size_t steps) {
	if(goal->weight == Blocked) {
		Console::WriteLine << "Goal is blocked!";
		return HexPath();
	}

	TraversalResult search = breadthFirstTraverse(goal, steps, steps);

	for(auto& vec : search.reachableTiles) {
		vec.Color(&layout, {1,0,0}, 0, ColorType::__X);
	}

	HexRegion shadow;
	for(auto& t : search.costMap) {
		if(t.second > (int) steps) { shadow.push_back(*t.first); }
	}
	shadow.Color(&layout, {0,0,1}, 0, ColorType::__T);

	int distance = start->DistanceFrom(goal);
	//Console::WriteLine << "Distance is " << distance << "  steps is " << steps;
	HexPath path;

	if(distance >= (int) steps) {
		Console::WriteLine << "Path is too far!";
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

HexPath HexGrid::CalculatePathWithinXCost(HexTile *const start, HexTile *const goal, size_t cost) {
	if(goal->weight == Blocked) {
		Console::WriteLine << "Goal is blocked!";
		return HexPath();
	}

	TraversalResult search = DijkstraTraverse(goal, cost, cost);

	for(auto& t : map) {
		t->DrawmX(layout, {t->weight / 4.0f, 0, 1}, 0);  //weights are 1-4 in fill
	}

	for(auto& vec : search.reachableTiles) {
		vec.Color(&layout, {0,1,1}, 0, ColorType::__X);
	}

	HexPath path;

	path.BuildPathReverse(start, goal, search.visitedMap);

	//Console::WriteLine << "Path has: " << path.size() << " elements.";
	//path.Color(&layout, {0,1,0}, 0.0f, ColorType::__mX);

	return path;
}

void HexGrid::step() {
	curPos++;
	if(curPos >= map.size() - 1) {
		curPos = 0;
		iter = map.begin();
	}

	std::advance(iter, 1);
}

HexRegion HexGrid::GetTilesNStepsAway(HexTile *const tile, int n) {
	int x = tile->q, y = tile->r, z = tile->s;

	HexRegion region = GetRegion(x - n, x + n, y - n, y + n, z - n, z + n);
	region.Filter(*this);

	return region;
}

HexRegion HexGrid::GetRegion(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax) {
	std::vector<HexTile> results = {};

	for(int x = xmin; x <= xmax; ++x) {
		for(int y = max(ymin, -x - zmax); y <= min(ymax, -x - zmin); ++y) {
			results.push_back(HexTile(x, y));
		}
	}

	return results;
}


HexRegion HexGrid::GetIntersectingTilesRanges(HexTile *const a, int aRadius, HexTile *const b, int bRadius) {
	int ax = a->q, ay = a->r, az = a->s;
	int bx = b->q, by = b->r, bz = b->s;

	HexRegion regionA = GetRegion(ax - aRadius, ax + aRadius, ay - aRadius, ay + aRadius, az - aRadius, az + aRadius);
	//regionA.Color(&layout, {0,1,0}, 0, ColorType::__X);

	HexRegion regionB = GetRegion(bx - bRadius, bx + bRadius, by - bRadius, by + bRadius, bz - bRadius, bz + bRadius);
	//regionB.Color(&layout, {0,0,1}, 0.01f, ColorType::__X);

	HexRegion intersection = GetRegion(
		max(ax - aRadius, bx - bRadius), min(ax + aRadius, bx + bRadius),
		max(ay - aRadius, by - bRadius), min(ay + aRadius, by + bRadius),
		max(az - aRadius, bz - bRadius), min(az + aRadius, bz + bRadius)
	);

	//intersection.Filter(*this);

	//intersection.Color(&layout, {1, 0, 0}, 0, ColorType::__CheapFill);

	return intersection;
}

HexPath HexGrid::breadthFirstSearch(HexTile *const start, HexTile *const goal) {
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

TraversalResult HexGrid::breadthFirstTraverse(HexTile *const tile, size_t steps, size_t maxMovement) {
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
				HexTile* neighbor = GetTileExact(n);
				if(!neighbor || neighbor->weight == Blocked) { continue; }

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

TraversalResult HexGrid::DijkstraTraverse(HexTile *const tile, size_t cost, size_t maxMovement) {
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
					HexTile* neighbor = GetTileExact(n);
					if(!neighbor || neighbor->weight == Blocked) { continue; }

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

HexPath HexGrid::DijkstraSearch(HexTile *const start, HexTile *const goal) {
	if(goal->weight == Blocked) return HexPath();

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
			HexTile* neighbor = GetTileExact(_n);
			if(!neighbor || neighbor->weight == Blocked) { continue; }
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

HexPath HexGrid::AStarSearch(const DirectX::XMFLOAT2& start, const DirectX::XMFLOAT2& goal, HeuristicsFunction Heuristic) {
	HexTile* s = GetTileExact((int) start.x, (int) start.y);
	HexTile* e = GetTileExact((int) goal.x, (int) goal.y);
	if(s && e) {
		return AStarSearch(s, e, Heuristic);
	}
	return HexPath();
}

HexPath HexGrid::AStarSearch(HexTile *const start, HexTile *const goal, HeuristicsFunction Heuristic) {
	if(goal->weight == Blocked) return HexPath();

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
			HexTile* neighbor = GetTileExact(_n);
			if(!neighbor || neighbor->weight == Blocked) { continue; }
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

void HexGrid::SetUpDrawingPaths() {
	iter = map.begin();
	HexTile* start = *iter;

	HexTile* istart = GetTileExact(0, 0);

	DrawLine += [=]() {
		HexTile* end = *iter;

		HexTile* realStart = const_cast<HexTile*&>(start);
		HexTile* realend = const_cast<HexTile*&>(end);

		auto sc = realStart->Center(layout);
		auto se = realend->Center(layout);

		DebugRenderer::AddLine({sc.x, sc.y,0}, {se.x, se.y,0}, {0,0,0});

		std::vector<HexTile> path = realStart->DrawSmoothLineTo(realend);

		//Console::WriteLine << "The path has " << path.size() << " elements.";

		for(auto& e : path) {
			e.Draw(layout, {1,0,0});
			e.DrawX(layout, {1,0,0});
		}

		realStart->Draw(layout, {0,0,1}, 0.1f);
		realend->Draw(layout, {0,1,0}, 0.1f);

		if(KeyIsDown(TestInputO)) {
			step();
			ResetKey(TestInputO);
			Console::WriteLine << "Current index is " << curPos;
		}
	};

	DrawRange += [=]() {
		HexTile* _start = *iter;

		HexTile* realStart = const_cast<HexTile*&>(_start);
		HexRegion range = GetTilesNStepsAway(realStart, 2);
		range.Filter(*this);
		//for(auto& e : path) {
		//	e->Draw(*layout, {1,0,0});
		//	e->Cross(*layout, {1,0,0});
		//}

		//range.Color(layout, {1,0,0}, 0.2f);

		realStart->Draw(layout, {0,0,1}, 0.1f);

		if(KeyIsDown(TestInputO)) {
			step();
			ResetKey(TestInputO);
			Console::WriteLine << "Current index is " << curPos;
		}
	};

	DrawIntersect += [=]() {
		HexTile* end = *iter;

		HexTile* realend = const_cast<HexTile*&>(end);

		HexTile* realStart = const_cast<HexTile*&>(istart);

		auto sc = realStart->Center(layout);
		auto se = realend->Center(layout);

		DebugRenderer::AddLine({sc.x, sc.y, 0}, {se.x, se.y, 0}, {0,0,0});

		GetIntersectingTilesRanges(realStart, 4, realend, 4);

		if(KeyIsDown(TestInputO)) {
			step();
			ResetKey(TestInputO);
			Console::WriteLine << "Current index is " << curPos;
		}
	};

	DrawXStepsPath += [=]() {
		HexTile* end = *iter;

		HexTile* realend = const_cast<HexTile*&>(end);
		HexTile* realStart = const_cast<HexTile*&>(istart);

		auto sc = realStart->Center(layout);
		auto se = realend->Center(layout);

		DebugRenderer::AddLine({sc.x, sc.y, 0}, {se.x, se.y, 0}, {0,0,0});
		realStart->DrawT(layout, {1,1,1}, 0.08f);
		realend->DrawT(layout, {1,1,1}, 0.08f);

		//CalculatePathWithinXSteps(realStart, realend, 8);
		CalculatePathWithinXCost(realStart, realend, 8);

		if(KeyIsHit(TestInputO)) {
			step();
			//	Console::WriteLine << "Current index is " << curPos;
		}
	};

	DrawAIPath += [=]() {
		HexTile* end = *iter;

		HexTile* realend = const_cast<HexTile*&>(end);
		HexTile* realStart = const_cast<HexTile*&>(start);

		auto sc = realStart->Center(layout);
		auto se = realend->Center(layout);

		DebugRenderer::AddLine({sc.x, sc.y, 0}, {se.x, se.y, 0}, {0,0,0});
		realStart->DrawT(layout, {1,1,1}, 0);
		realend->DrawT(layout, {1,1,1}, 0);

		//HexPath d = DijkstraSearch(realStart, realend);
	//	HexPath p = AStarSearch(realStart, realend, Heuristics::ManhattanDistance);
		//p.Color(&layout, {1,0,0}, 0, ColorType::__CheapFill);
		//Console::WriteLine << "Path contains " << p.size() << " elements.";

		if(KeyIsHit(TestInputO)) {
			step();
			//	Console::WriteLine << "Current index is " << curPos;
		}
	};
}

bool HexGrid::IsBlocked(HexTile* tile){
	return tile?tile->weight == Blocked:true;
}
bool HexGrid::IsBlocked(const DirectX::XMFLOAT2& pos) {
	return IsBlocked(PointToTile(pos));
}
bool HexGrid::IsBlocked(const float x, const float z) {
	return IsBlocked(DirectX::XMFLOAT2(x, z));
}
void HexGrid::Fill() {
	srand((unsigned) time(0));
	for(int q = (int) -map_radius; q <= map_radius; ++q) {
		int r1 = (int) max(-map_radius, -q - map_radius);
		int r2 = (int) min(map_radius, -q + map_radius);
		for(int r = r1; r <= r2; ++r) {
			HexTile* t = new HexTile(q, r);
			t->weight = 1;
			//if(rand() % 100 < 40) {
			//	t->weight = (float) Blocked;
			///} else {
			//	t->weight = float(rand() % 4) + 1;
			//}
			map.insert(t);
		}
	}

	//HexTile* start = GetTileExact(0, 0);
	//start->weight = 1.0f;

	for(auto& t : map) {
		if(t->weight == Blocked)
			blocked.push_back(*t);
	}

	//	SetUpDrawingPaths();
}

HexRegion HexGrid::blocked;

void HexGrid::Display(DirectX::XMFLOAT2& player) {
	HexTile* pos = PointToTile(player);
	if(pos) {
		HexRegion playerView = GetTilesNStepsAway(pos, 3);
		playerView.Color(&layout, {1,1,1}, 0, ColorType::__Outline);
	}

//	for(const auto& t : map) {
	//	auto realT = const_cast<HexTile*&>(t);
	//	realT->Draw(layout, {1,1,1});
	//}

	
	//blocked.Color(&layout, {0,0,0}, 0, ColorType::__CheapFill);

	//DrawXStepsPath();
}

HexGrid::~HexGrid() {
	for(HexTile* t : map) {
		delete t;
		t = nullptr;
	}
	map.clear();
}

HexTile* HexGrid::GetRandomTile() {
	auto begin = map.begin();
	const unsigned long n = (unsigned long)std::distance(begin, map.end());
	const unsigned long divisor = (unsigned long) ((RAND_MAX + 1) / n);

	unsigned long k;
	do { k = std::rand() / divisor; } while(k >= n);

	std::advance(begin, k);
	return *begin;
}


HexTile* HexGrid::GetTileExact(const int x, const int y) {
	auto it = map.find(&HexTile(x, y));
	return it == map.end() ? nullptr : *it;
}

HexTile* HexGrid::GetTileExact(HexTile& t) const {
	auto it = map.find(&t);
	return it == map.end() ? nullptr : *it;
}
