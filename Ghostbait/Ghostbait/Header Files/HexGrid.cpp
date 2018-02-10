#include "HexGrid.h"

#include "HexagonalGridLayout.h"
#include "HexagonTile.h"

#include <unordered_map>
#include "Console.h"
#include "DebugRenderer.h"

#include <chrono>
#include <thread>

#include <queue>

void HexGrid::step() {
	curPos++;
	if(curPos >= map.size() - 1) {
		curPos = 0;
		iter = map.begin();
	}

	std::advance(iter, 1);
}

HexGrid::HexRegion HexGrid::GetTilesNStepsAway(HexTile* tile, int n) {
	int x = tile->q, y = tile->r, z = tile->s;

	HexRegion region = GetRegion(x-n, x+n, y-n, y+n, z-n, z+n);

	return region;
}

HexGrid::HexRegion HexGrid::GetRegion(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax) {
	std::vector<HexTile> results = {};

	for(int x = xmin; x <= xmax; ++x) {
		for(int y = max(ymin, -x - zmax); y <= min(ymax, -x - zmin); ++y) {
			results.push_back(HexTile(x, y));
		}
	}

	HexRegion r;
	r.tiles = results;
	return r;
}

void HexGrid::ColorRegion(HexRegion region, HexagonalGridLayout* layout, DirectX::XMFLOAT3 color, float offset) {
	for(auto& e : region.tiles) {
		e.Cross(*layout, color, offset);
	}
}


std::vector<HexTile*> HexGrid::GetIntersectingTilesRanges(HexagonalGridLayout* layout, HexTile* a, int aN, HexTile* b, int bN) {
	std::vector<HexTile*> results = {};

	int ax = a->q, ay = a->r, az = a->s;
	int bx = b->q, by = b->r, bz = b->s;

	HexRegion regionA = GetRegion(ax - aN, ax + aN, ay - aN, ay + aN, az - aN, az + aN);
	ColorRegion(regionA, layout, {0,1,0}, 0);

	HexRegion regionB = GetRegion(bx - bN, bx + bN, by - bN, by + bN, bz - bN, bz + bN);
	ColorRegion(regionB, layout, {0,0,1}, 0.01f);

	HexRegion intersection = GetRegion(
		max(ax - aN, bx - bN),	min(ax+aN, bx + bN),
		max(ay - aN, by - bN),	min(ay+aN, by + bN),
		max(az - aN, bz - bN),	min(az+aN, bz + bN)
	);

	ColorRegion(intersection, layout, {1, 0, 0}, 0.22f);

	return results;
}

std::vector<HexTile*> HexGrid::breadthFirstSearch(HexTile * start, HexTile * goal) {
	std::queue<HexTile*> Q;
	std::unordered_map<HexTile*, HexTile*> visited;

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

std::vector<HexTile*> HexGrid::DijkstraSearch(HexTile * start, HexTile * goal) {
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

		ColorRegion(range, layout, {1,0,0}, 0.2f);

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

}

void HexGrid::Fill() {
	for(int q = (int) -map_radius; q <= map_radius; q++) {
		int r1 = (int) max(-map_radius, -q - map_radius);
		int r2 = (int) min(map_radius, -q + map_radius);
		for(int r = r1; r <= r2; r++) {
			map.insert(new HexTile(q, r));
		}
	}
	SetUpDrawingPaths();
}

void HexGrid::Display(HexagonalGridLayout layout) {
	for(const auto& t : map) {
		auto realT = const_cast<HexTile*&>(t);

		realT->Draw(layout, {1,1,1});
	}

	DrawRange(&layout);
}

HexGrid::~HexGrid() {
	for(HexTile * t : map) {
		delete t;
		t = nullptr;
	}
	map.clear();
}
