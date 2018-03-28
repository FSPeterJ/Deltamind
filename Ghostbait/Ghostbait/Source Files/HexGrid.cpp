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
#include "PathPlanner.h"
#include <fstream>
using namespace Common;

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

HexGrid::HexGrid(float _radius, const HexagonalGridLayout& _layout) : map_radius(_radius), layout(_layout) {}

HexTile* HexGrid::PointToTile(const DirectX::XMFLOAT2& p) {
	return GetTileExact(HexTile::Round(HexagonalGridLayout::PointToHexagonTile(p, layout)));
}

HexTile HexGrid::PointToTileOffGrid(const DirectX::XMFLOAT2& p) {
	return HexTile::Round(HexagonalGridLayout::PointToHexagonTile(p, layout));
}

DirectX::XMFLOAT2 HexGrid::TileToPoint(HexTile * tile) {
	return tile->Center(layout);
}

HexTile* HexGrid::GetTileExact(const int x, const int y) {
	auto it = map.find(&HexTile(x, y));
	return it == map.end() ? nullptr : *it;
}

HexTile* HexGrid::GetTileExact(HexTile& t) const {
	auto it = map.find(&t);
	return it == map.end() ? nullptr : *it;
}

bool HexGrid::IsBlocked(HexTile* tile) {
	return tile ? tile->weight == Blocked : true;
}
bool HexGrid::IsBlocked(const DirectX::XMFLOAT2& pos) {
	return IsBlocked(PointToTile(pos));
}
bool HexGrid::IsBlocked(const float x, const float z) {
	return IsBlocked(DirectX::XMFLOAT2(x, z));
}

bool HexGrid::SetWeight(const DirectX::XMFLOAT2& tilePosition, float weight) {
	HexTile* t = PointToTile(tilePosition);
	return SetWeight(t, weight);
}

bool HexGrid::SetWeight(HexTile *const tile, float weight) {
	if(tile) {
		//cost_delta[tile] = tile->weight;
		PathPlanner::CostChangeNotice(tile);
		tile->weight = weight;
		return true;
	}
	return false;
}

bool HexGrid::AddObstacle(const DirectX::XMFLOAT2& obstaclePosition) {
	HexTile* t = PointToTile(obstaclePosition);
	return AddObstacle(t);
}
bool HexGrid::AddObstacle(HexTile*const obstaclePosition) {
	if (obstaclePosition) {
		Console::WriteLine << "Obstacle added: Tile (" << obstaclePosition->q << ", " << obstaclePosition->r << ")";
		PathPlanner::CostChangeNotice(obstaclePosition);
		//cost_delta[obstaclePosition] = obstaclePosition->weight;
		obstaclePosition->weight = Blocked;
		blocked.push_back(*obstaclePosition);
		return true;
	}
	return false;
}


bool HexGrid::RemoveObstacle(const DirectX::XMFLOAT2& obstaclePosition) {
	HexTile* t = PointToTile(obstaclePosition);
	return RemoveObstacle(t);
}
bool HexGrid::RemoveObstacle(HexTile*const obstaclePosition) {
	if (obstaclePosition) {
		PathPlanner::CostChangeNotice(obstaclePosition);
		//cost_delta[obstaclePosition] = obstaclePosition->weight;
		obstaclePosition->weight = 1;
		blocked.remove(*obstaclePosition);
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

		//if(KeyIsDown(TestInputO)) {
		//	step();
		//	ResetKey(TestInputO);
		//	Console::WriteLine << "Current index is " << curPos;
		//}
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

		//if(KeyIsDown(TestInputO)) {
		//	step();
		//	ResetKey(TestInputO);
		//	Console::WriteLine << "Current index is " << curPos;
		//}
	};

	DrawIntersect += [=]() {
		HexTile* end = *iter;

		HexTile* realend = const_cast<HexTile*&>(end);

		HexTile* realStart = const_cast<HexTile*&>(istart);

		auto sc = realStart->Center(layout);
		auto se = realend->Center(layout);

		DebugRenderer::AddLine({sc.x, sc.y, 0}, {se.x, se.y, 0}, {0,0,0});

		GetIntersectingTilesRanges(realStart, 4, realend, 4);

		//if(KeyIsDown(TestInputO)) {
		//	step();
		//	ResetKey(TestInputO);
		//	Console::WriteLine << "Current index is " << curPos;
		//}
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
		//CalculatePathWithinXCost(realStart, realend, 8);

		//if(KeyIsHit(TestInputO)) {
		//	step();
			//	Console::WriteLine << "Current index is " << curPos;
		//}
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

		//if(KeyIsHit(TestInputO)) {
		//	step();
			//	Console::WriteLine << "Current index is " << curPos;
		//}
	};
}

void HexGrid::Fill(bool withRandomObstacles) {
	srand((unsigned) time(0));
	for(int q = (int) -map_radius; q <= map_radius; ++q) {
		int r1 = (int) max(-map_radius, -q - map_radius);
		int r2 = (int) min(map_radius, -q + map_radius);
		for(int r = r1; r <= r2; ++r) {
			HexTile* t = new HexTile(q, r);
			if(withRandomObstacles) {
				if(rand() % 100 < 25) {
					t->weight = (float) Blocked;
				} else {
					t->weight = float(rand() % 4) + 1;
				}
			} else {
				t->weight = 1;
			}

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
	//HexTile* pos = PointToTile(player);
	//if(pos) {
	//	HexRegion playerView = GetTilesNStepsAway(pos, 3);
	//	playerView.Color(&layout, {1,1,1}, 0, ColorType::__Outline);
	//} else {
	//	HexRegion playerView = GetTilesNStepsAway(&HexTile(0, 0), 13);
	//	playerView.Color(&layout, {1,1,1}, 0, ColorType::__Outline);
	//}

	
	//draw everything
	for(const auto& t : map) {
		auto realT = const_cast<HexTile*&>(t);
		realT->DrawX(layout, {1,1,1}, 0.2f);
	}
	blocked.Color(&layout, {0,0,0}, 0, ColorType::__CheapFill);

	//DrawXStepsPath();
}

void HexGrid::ForEach(std::function<void(HexTile*const)> f) {
	for(auto& t : map) {
		f(t);
	}
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
	const unsigned long n = (unsigned long) std::distance(begin, map.end());
	const unsigned long divisor = (unsigned long) ((RAND_MAX + 1) / n);

	unsigned long k;
	do { k = std::rand() / divisor; } while(k >= n);

	std::advance(begin, k);
	return *begin;
}

HexGrid::HexGrid(const char* _filename, float _radius, HexagonalGridLayout _layout) : map_radius(_radius), layout(_layout) {
	static int dupCount = 0;
	std::fstream file;

	file.open(_filename, std::ios_base::binary | std::ios_base::in);

	if (file.is_open())
	{
		int geo_count;
		file.read((char*)&geo_count, sizeof(geo_count));

		for (int i = 0; i < geo_count; ++i)
		{
			unsigned int polyCount;
			file.read((char*)&polyCount, sizeof(polyCount));

			for (unsigned int i = 0; i < polyCount; ++i)
			{
				int q, r, s;
				file.read((char*)&q, sizeof(int));
				file.read((char*)&r, sizeof(int));
				file.read((char*)&s, sizeof(int));

				HexTile* t = new HexTile(q, r, s);
				t->weight = 1.0f;
				if (q == -15 && r == 34) {
					int i = 0;
				}
				if (map.find(t) != map.end()) {
					++dupCount;
				}
				map.insert(t);
			}
		}
		file.close();
		
	}

	//int i = 0;
	//HexTile tile = HexTile(-15, 34);
	//DirectX::XMFLOAT2 pos = tile.Center(HexagonalGridLayout::FlatLayout);
}
void HexGrid::Color(HexRegion& r, DirectX::XMFLOAT3 color, int fill) {
	r.Color(&layout, color, 0, (ColorType)fill);
}

void HexGrid::Color(HexPath& p, DirectX::XMFLOAT3 color, int fill) {
	p.Color(&layout, color, 0, (ColorType)fill);
}

HexRegion HexGrid::DoRing(bool spiral, HexTile *const center, std::size_t radius) {
	HexRegion ring;
	if (radius == 0) { return ring; }

	if (spiral) { ring.push_back(*center); }
	for (std::size_t k = spiral ? 1 : radius; k <= radius; ++k) {
		HexTile H = *center + (center->Direction(NEIGHBOR_DIRECTION::BottomLeft) * (int)k);
		for (std::size_t i = 0; i < Hexagon::NUMBER_OF_SIDES; ++i) {
			for (std::size_t j = 0; j < k; ++j) {
				ring.push_back(H);
				H = H.Neighbor((NEIGHBOR_DIRECTION)i);
			}
		}
	}
	return ring;
}

HexRegion HexGrid::Spiral(HexTile *const center, std::size_t radius) {
	return DoRing(true, center, radius);
}

HexRegion HexGrid::Ring(HexTile *const center, std::size_t radius) {
	return DoRing(false, center, radius);
}

bool HexGrid::AddWeight(HexTile *const tile, float value) {
	if (!tile)	return false;
	if (IsBlocked(tile)) return false;

	float result = tile->weight + value;
	if (result >= Blocked) return false;
	SetWeight(tile, result);
	return true;
}
