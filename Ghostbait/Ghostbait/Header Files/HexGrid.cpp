#include "HexGrid.h"

#include "HexagonalGridLayout.h"
#include "HexagonTile.h"

#include <unordered_map>
#include "Console.h"
#include "DebugRenderer.h"

#include <chrono>
#include <thread>

#include <queue>

std::vector<HexTile*> HexGrid::GetTilesNStepsAway(HexTile* tile, size_t n) {
	std::vector<HexTile*> results = {};




	//std::vector<HexTile*> tiles = grid.

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

	Draw += [=](HexagonalGridLayout* layout) {
		HexTile* end = *iter;

		HexTile* realStart = const_cast<HexTile*&>(start);
		HexTile* realend = const_cast<HexTile*&>(end);

		auto sc = realStart->Center(*layout);
		auto se = realend->Center(*layout);

		DebugRenderer::AddLine({sc.x, sc.y,0}, {se.x, se.y,0}, {0,0,0});

		std::vector<HexTile> path = realStart->DrawSmoothLineTo(realend);

		Console::WriteLine << "The path has " << path.size() << " elements.";

		for(auto& e : path) {
			e.Draw(*layout, {1,0,0});
			e.Cross(*layout, {1,0,0});
		}

		realStart->Draw(*layout, {0,0,1}, 0.1f);
		realend->Draw(*layout, {0,1,0}, 0.1f);

		curPos++;
		if(curPos >= map.size() - 1) {
			curPos = 0;
			iter = map.begin();
		}

		std::advance(iter, 1);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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

	Draw(&layout);
}

HexGrid::~HexGrid() {
	for(HexTile * t : map) {
		delete t;
		t = nullptr;
	}
	map.clear();
}
