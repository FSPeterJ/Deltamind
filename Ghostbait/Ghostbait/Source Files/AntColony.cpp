#include "AntColony.h"
//#include "PathPlanner.h"
#include "HexGrid.h"
#include "GhostTime.h"
#include "EngineStructure.h"

AntColony::AntColony(HexGrid* _grid) : activeGrid(_grid) {
	updateID = EngineStructure::Update.Add([=]() { Update(); });
};

void AntColony::LeavePheromone(Pheromone& _pheromone) {
	activeGrid->AddWeight(_pheromone.tile, _pheromone.lingerTime * _pheromone.scentStrength);
	trails.push_back(_pheromone);
}

void AntColony::ClearTrails() {
	for (size_t i = 0; i < trails.size(); ++i) {
		activeGrid->AddWeight(trails[i].tile, trails[i].lingerTime * trails[i].scentStrength);
	}
}

void AntColony::Update() {
	float delta = (float)GhostTime::DeltaTime();
	
	std::vector<Pheromone>::iterator itr = trails.begin();
	while (itr != trails.end()) {
		if (delta >= itr->lingerTime) {
			activeGrid->AddWeight(itr->tile, -(itr->lingerTime * itr->scentStrength));
			std::swap(itr, trails.end() - 1);
			trails.pop_back();
		}
		else {
			activeGrid->AddWeight(itr->tile, -(delta * itr->scentStrength));
			itr->lingerTime -= delta;
			++itr;
		}
	}
}