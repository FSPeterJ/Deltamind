#include "HexTileVector.h"
#include "HexGrid.h"

HexTile* HexRegion::getData() { return &data[0]; }

HexRegion::HexRegion(std::vector<HexTile>& that) { data.swap(that); }

std::vector<HexTile>::iterator HexRegion::remove(const HexTile & v) { return data.erase(std::remove(data.begin(), data.end(), v), data.end()); }

void HexRegion::push_back(const HexTile & v) { data.emplace_back(v); }

void HexRegion::push_back(HexTile && v) { data.emplace_back(std::move(v)); }

void HexRegion::Color(HexagonalGridLayout * const layout, DirectX::XMFLOAT3 color, float offset, ColorType c) {
	switch(c) {
	case ColorType::__X:
		for(auto& e : data) {
			e.DrawX(*layout, color, offset);
		}
		break;
	case ColorType::__T:
		for(auto& e : data) {
			e.DrawT(*layout, color, offset);
		}
		break;
	case ColorType::__mX:
		for(auto& e : data) {
			e.DrawmX(*layout, color, offset);
		}
		break;
	case ColorType::__CheapFill:
		for(auto& e : data) {
			e.DrawCheapFill(*layout, color, offset);
		}
		break;
	case ColorType::__Outline:
		for(auto& e : data) {
			e.Draw(*layout, color, offset, e.weight == HexGrid::Blocked);
		}
		break;
	}
}

void HexRegion::Filter(HexGrid & grid) {
	//should this be a function in hexgrid that gets passed a region?
	if(data.size()) {
		for(auto it = data.begin(); it != data.end();) {
			HexTile* t = grid.GetTile(*it);
			if(!t) {
				it = remove(*it);
			} else {
				++it;
			}
		}
	}
}

HexTile** HexPath::getData() { return &data[0]; }


HexTile* HexPath::goal() { return data.back(); }
HexTile* HexPath::start() { return data.front(); }

HexPath::HexPath(std::vector<HexTilePtr>& that) { data.swap(that); }

std::vector<HexTile*>::iterator HexPath::remove(const HexTilePtr & v) { return data.erase(std::remove(data.begin(), data.end(), v), data.end()); }

void HexPath::push_back(const HexTilePtr & v) { data.emplace_back(v); }

void HexPath::push_back(HexTilePtr && v) { data.emplace_back(std::move(v)); }

float HexPath::cost() const {
	float c = 0;
	//for(TileType t : data) { c += t.weight; }
	return c;
}

void HexPath::Color(HexagonalGridLayout * const layout, DirectX::XMFLOAT3 color, float offset, ColorType c) {
	switch(c) {
	case ColorType::__X:
		for(auto& e : data) {
			e->DrawX(*layout, color, offset);
		}
		break;
	case ColorType::__T:
		for(auto& e : data) {
			e->DrawT(*layout, color, offset);
		}
		break;
	case ColorType::__mX:
		for(auto& e : data) {
			e->DrawmX(*layout, color, offset);
		}
		break;
	case ColorType::__CheapFill:
		for(auto& e : data) {
			e->DrawCheapFill(*layout, color, offset);
		}
		break;
	case ColorType::__Outline:
		for(auto& e : data) {
			e->Draw(*layout, color, offset, e->weight == HexGrid::Blocked);
		}
		break;
	}
}

void HexPath::BuildPath(HexTilePtr const start, HexTilePtr const goal, VisitedMap& came_from) {

	HexTilePtr current = goal;

	while(current != start && current) {
		data.push_back(current);
		current = came_from[current];
	}

	data.push_back(start);
	std::reverse(data.begin(), data.end());
}

void HexPath::BuildPathReverse(HexTilePtr const start, HexTilePtr const goal, VisitedMap& came_from) {
	HexTilePtr current = start;

	while(current != goal && current) {
		data.push_back(current);
		current = came_from[current];
	}

	data.push_back(goal);
	//std::reverse(data.begin(), data.end());
}
