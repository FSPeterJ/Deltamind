#include "AntColony.h"
//#include "PathPlanner.h"
#include "HexGrid.h"
#include "GhostTime.h"
#include "EngineStructure.h"

AntProperties::AntProperties() {};
AntProperties::AntProperties(float _lingerTime, float _scentStrength) : lingerTime(_lingerTime), scentStrength(_scentStrength) {};
AntProperties::AntProperties(const AntProperties& _other) : lingerTime(_other.lingerTime), scentStrength(_other.scentStrength) {};
AntProperties& AntProperties::operator=(const AntProperties& _other) { 
	lingerTime = _other.lingerTime; 
	scentStrength = _other.scentStrength; 
	return *this; 
}

Pheromone::Pheromone() {};
Pheromone::Pheromone(HexTile* const _tile, const AntProperties& _prop) : tile(_tile), prop(_prop) {};
Pheromone::Pheromone(HexTile* const _tile, float _lingerTime, float _scentStrength) : tile(_tile), prop(AntProperties(_lingerTime, _scentStrength)) {};
Pheromone& Pheromone::operator=(const Pheromone& _other) { 
	tile = _other.tile; 
	prop = _other.prop; 
	return *this; 
};


HexGrid* AntColony::activeGrid = nullptr;
std::vector<Pheromone> AntColony::trails;
unsigned AntColony::updateID = 0;

void AntColony::SetGrid(HexGrid* _grid) {
	activeGrid = _grid;
}

void AntColony::AddUpdateToEngineStruct() {
	updateID = EngineStructure::Update.Add([=]() { Update(); });
}

void AntColony::RemoveUpdateFromEngineStruct() {
	if (updateID)
		EngineStructure::Update.Remove(updateID);
}

void AntColony::LeavePheromone(const Pheromone& _pheromone) {
	activeGrid->AddWeight(_pheromone.tile, _pheromone.prop.lingerTime * _pheromone.prop.scentStrength);
	trails.push_back(_pheromone);
}

void AntColony::LeavePheromone(HexTile* const _tile, const AntProperties& _prop) {
	activeGrid->AddWeight(_tile, _prop.lingerTime * _prop.scentStrength);
	trails.push_back(Pheromone(_tile, _prop));
}

void AntColony::LeavePheromone(HexTile* const _tile, float _lingerTime, float _scentStrength) {
	activeGrid->AddWeight(_tile, _lingerTime * _scentStrength);
	trails.push_back(Pheromone(_tile, _lingerTime, _scentStrength));
}

void AntColony::ClearTrails() {
	for (size_t i = 0; i < trails.size(); ++i) {
		activeGrid->AddWeight(trails[i].tile, trails[i].prop.lingerTime * trails[i].prop.scentStrength);
	}
}

void AntColony::Update() {
	float delta = (float)GhostTime::DeltaTime();
	
	size_t index = 0;
	Pheromone* currPtr;
	while (index < trails.size()) {
		currPtr = &trails[index];
		if (delta >= currPtr->prop.lingerTime) {
			activeGrid->AddWeight(currPtr->tile, -(currPtr->prop.lingerTime * currPtr->prop.scentStrength));
			
			*currPtr = trails.back();
			trails.pop_back();
		}
		else {
			activeGrid->AddWeight(currPtr->tile, -(delta * currPtr->prop.scentStrength));
			currPtr->prop.lingerTime -= delta;
			++index;
		}
	}
}