#include "HexGrid.h"

#include "HexagonalGridLayout.h"
#include "HexagonTile.h"

#include <unordered_map>
#include "Console.h"
#include "DebugRenderer.h"

#include <chrono>
#include <thread>

#include <queue>

#include <vector>


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

bool EqualComparator::operator()(const HexTile* lhs, const HexTile* rhs) const {
	return *lhs == *rhs;
}
bool EqualComparator::operator()(const HexTile& lhs, const HexTile& rhs) const {
	return lhs == rhs;
}

class HexRegion {
	std::vector<HexTile> data;
public:
	void Color(HexagonalGridLayout *const layout, DirectX::XMFLOAT3 color, float offset) {
		for(auto& e : data) {
			e.Cross(*layout, color, offset);
		}
	}

	HexRegion() {}

	HexRegion(std::vector<HexTile>& that) {
		data.swap(that);
	}

	HexRegion& operator=(std::vector<HexTile>& that) {
		data.swap(that);
		return *this;
	}

	HexRegion& operator=(HexRegion& that) {
		data.swap(that.data);
		return *this;
	}

	void push_back(const HexTile& _Val) {
		data.emplace_back(_Val);
	}

	void push_back(HexTile&& _Val) {
		data.emplace_back(_STD move(_Val));
	}
};


std::vector<HexTile*> HexGrid::CalculatePathWithinXSteps(HexTile *const tile, HexTile *const goal, size_t steps, HexagonalGridLayout *const layout) {
	BreadthTraversalResult search = breadthFirstTraverse(goal, steps, steps);

	for(auto& vec : search.reachableTiles) {
		for(auto& _t : vec) {
			_t->Cross(*layout, {0,1,0}, 0.3f);
		}
	}

	HexRegion blocked;
	for(auto& _t : map) {
		if(_t->cost == 0)
			blocked.push_back(*_t);
	}
	blocked.Color(layout, {0,0,0}, 0.2f);

	HexRegion shadow;
	for(auto& t : search.cost_so_far) {
		if(t.second > steps) {
			shadow.push_back(*t.first);
		}
	}
	shadow.Color(layout, {1,0,1}, 0.2f);

	std::vector<HexTile*> path;

	HexTile* current = tile;

	while(current != goal && current) {
		path.push_back(current);
		current = search.came_from[current];
	}

	for(auto& e : path) {
		e->Cross(*layout, {1,0,0}, 0.1f);
	}

	step();
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

std::vector<HexTile*> HexGrid::GetIntersectingTilesRanges(HexagonalGridLayout *const layout, HexTile *const a, int aN, HexTile *const b, int bN) {
	std::vector<HexTile*> results = {};

	int ax = a->q, ay = a->r, az = a->s;
	int bx = b->q, by = b->r, bz = b->s;

	HexRegion regionA = GetRegion(ax - aN, ax + aN, ay - aN, ay + aN, az - aN, az + aN);
	regionA.Color(layout, {0,1,0}, 0);

	HexRegion regionB = GetRegion(bx - bN, bx + bN, by - bN, by + bN, bz - bN, bz + bN);
	regionB.Color(layout, {0,0,1}, 0.01f);

	HexRegion intersection = GetRegion(
		max(ax - aN, bx - bN), min(ax + aN, bx + bN),
		max(ay - aN, by - bN), min(ay + aN, by + bN),
		max(az - aN, bz - bN), min(az + aN, bz + bN)
	);

	intersection.Color(layout, {1, 0, 0}, 0.22f);

	return results;
}

std::vector<HexTile*> HexGrid::breadthFirstSearch(HexTile *const start, HexTile *const goal) {
	std::queue<HexTile*> Q;
	VisitedMap visited;

	Q.push(start);
	visited[start] = nullptr;

	while(!Q.empty()) {
		HexTile* current = Q.front();
		Q.pop();

		if(current == goal) {
			break;
		}

		std::vector<HexTile> curNeighbors = current->Neighbors();

		for(auto& neighbor : curNeighbors) {
			if(!visited.count(&neighbor)) {
				Q.push(&neighbor);
				visited[&neighbor] = current;
			}
		}
	}

	std::vector<HexTile*> path;

	HexTile* current = goal;

	while(current != start) {
		path.push_back(current);
		current = visited[current];
	}

	//path.push_back(start);
	//path.reverse();

	return path;
}

BreadthTraversalResult HexGrid::breadthFirstTraverse(HexTile *const tile, size_t steps, size_t maxMovement) {
	VisitedMap came_from;
	came_from[tile] = nullptr;

	CostMap step_cost_so_far;
	step_cost_so_far[tile] = 0;

	std::vector<std::vector<HexTile*>> outerEdges = {};
	outerEdges.resize(steps);
	outerEdges[0].push_back(tile);

	for(size_t i = 0; i < steps-1 && outerEdges[i].size(); ++i) {
		for(size_t j = 0; j < outerEdges[i].size(); ++j) {
			const HexTile* _this = outerEdges[i][j];
			for(size_t n = 0; n < 6; ++n) {
				HexTile _calcNeighbor = *_this + directions[n];
				HexTile* neighbor = nullptr;


				auto it = map.find(&_calcNeighbor);
			

				for(auto& t : map) {
					if(*t == _calcNeighbor) {
						neighbor = t;
						break;
					}
				}

				if(neighbor == nullptr) {
					continue;
				}

				bool contain = step_cost_so_far.count(neighbor) > 0;
				bool withinReach = tile->DistanceFrom(neighbor) <= steps;
				bool notBlocked = neighbor->cost;

				if(!contain && notBlocked && withinReach) {
					step_cost_so_far[neighbor] = int(i + 1);
					outerEdges[i + 1].push_back(neighbor);
					came_from[neighbor] = const_cast<HexTile*>(_this);
				}

			}
		}
	}

	BreadthTraversalResult res;
	res.came_from = came_from;
	res.cost_so_far = step_cost_so_far;
	res.reachableTiles = outerEdges;

	return res;
}

std::vector<HexTile*> HexGrid::DijkstraSearch(HexTile *const start, HexTile *const goal) {
	//	std::priority_queue<HexTile*, std::vector<HexTile*>, CostComparator> Q;
	//	std::unordered_map<HexTile*, HexTile*> visited;
	//	std::unordered_map<HexTile*, float> cumulativeCost;
	//
	//	Q.push(start);
	//	visited[start] = nullptr;
	//	cumulativeCost[start] = 0;
	//
	//	while(!Q.empty()) {
	//		HexTile* current = Q.top();
	//		Q.pop();
	//
	//		if(current == goal) {
	//			break;
	//		}
	//
	//		std::vector<HexTile> curNeighbors = current->Neighbors();
	//
	//		for( auto&   neighbor : curNeighbors) {
	//			float curCost = cumulativeCost[current] + neighbor.cost;
	//
	//			if(cumulativeCost.count(&neighbor) <= 0 || curCost < cumulativeCost[&neighbor]) {
	//				cumulativeCost[&neighbor] = curCost;
	//
	//				HexTile* tmp = &neighbor;
	//				tmp->cost = curCost;
	//				Q.push(tmp);
	//
	//				visited[&neighbor] = current;
	//			}
	//		}
	//	}
	//
	std::vector<HexTile*> path;
	//
	//	HexTile* current = goal;
	//
	//	while(current != start) {
	//		path.push_back(current);
	//		current = visited[current];
	//	}
	//
	//	//path.push_back(start);
	//	//path.reverse();
	//
	return path;
}

void HexGrid::SetUpDrawingPaths() {
	iter = map.begin();
	HexTile* start = *iter;

	std::advance(iter, 8);
	HexTile* istart = *iter;
	iter = map.begin();

	//start 2 1 -3
	//end -3 1 2

	DrawLine += [=](HexagonalGridLayout* layout) {
		HexTile* end = *iter;

		HexTile* realStart = const_cast<HexTile*&>(start);
		HexTile* realend = const_cast<HexTile*&>(end);

		auto sc = realStart->Center(*layout);
		auto se = realend->Center(*layout);

		DebugRenderer::AddLine({sc.x, sc.y,0}, {se.x, se.y,0}, {0,0,0});

		std::vector<HexTile> path = realStart->DrawSmoothLineTo(realend);

		//Console::WriteLine << "The path has " << path.size() << " elements.";

		for(auto& e : path) {
			e.Draw(*layout, {1,0,0});
			e.Cross(*layout, {1,0,0});
		}

		realStart->Draw(*layout, {0,0,1}, 0.1f);
		realend->Draw(*layout, {0,1,0}, 0.1f);

		if(KeyIsDown(TestInputO)) {
			step();
			ResetKey(TestInputO);
			Console::WriteLine << "Current index is " << curPos;
		}
	};

	DrawRange += [=](HexagonalGridLayout* layout) {
		HexTile* _start = *iter;

		HexTile* realStart = const_cast<HexTile*&>(_start);
		HexRegion range = GetTilesNStepsAway(realStart, 2);

		//for(auto& e : path) {
		//	e->Draw(*layout, {1,0,0});
		//	e->Cross(*layout, {1,0,0});
		//}

		//range.Color(layout, {1,0,0}, 0.2f);

		realStart->Draw(*layout, {0,0,1}, 0.1f);

		if(KeyIsDown(TestInputO)) {
			step();
			ResetKey(TestInputO);
			Console::WriteLine << "Current index is " << curPos;
		}
	};

	DrawIntersect += [=](HexagonalGridLayout* layout) {
		HexTile* end = *iter;

		HexTile* realend = const_cast<HexTile*&>(end);

		HexTile* realStart = const_cast<HexTile*&>(istart);

		auto sc = realStart->Center(*layout);
		auto se = realend->Center(*layout);

		DebugRenderer::AddLine({sc.x, sc.y, 0}, {se.x, se.y, 0}, {0,0,0});

		GetIntersectingTilesRanges(layout, realStart, 2, realend, 2);

		if(KeyIsDown(TestInputO)) {
			step();
			ResetKey(TestInputO);
			Console::WriteLine << "Current index is " << curPos;
		}
	};

	DrawXStepsPath += [=](HexagonalGridLayout* layout) {
		HexTile* end = *iter;

		HexTile* realend = const_cast<HexTile*&>(end);
		HexTile* realStart = const_cast<HexTile*&>(start);

		auto sc = realStart->Center(*layout);
		auto se = realend->Center(*layout);

		DebugRenderer::AddLine({sc.x, sc.y, 0}, {se.x, se.y, 0}, {0,0,0});
		realStart->Star(*layout, {1,1,1}, 1);
		realend->Star(*layout, {1,1,1}, 1);

		CalculatePathWithinXSteps(realStart, realend, 5, layout);

		if(KeyIsDown(TestInputO)) {
			step();
			ResetKey(TestInputO);
			Console::WriteLine << "Current index is " << curPos;
		}
	};
}

void HexGrid::Fill() {
	srand(0);
	for(int q = (int) -map_radius; q <= map_radius; q++) {
		int r1 = (int) max(-map_radius, -q - map_radius);
		int r2 = (int) min(map_radius, -q + map_radius);
		for(int r = r1; r <= r2; r++) {
			HexTile* t = new HexTile(q, r);
			if(rand() % 100 < 30) {
				t->cost = 0.0f;
			} else {
				t->cost = 1.0f;
			}
			map.insert(t);
		}
	}
	SetUpDrawingPaths();
}

void HexGrid::Display(HexagonalGridLayout layout) {
	for(const auto& t : map) {
		auto realT = const_cast<HexTile*&>(t);

		realT->Draw(layout, {1,1,1});
	}

	DrawXStepsPath(&layout);
}

HexGrid::~HexGrid() {
	for(HexTile * t : map) {
		delete t;
		t = nullptr;
	}
	map.clear();
}
