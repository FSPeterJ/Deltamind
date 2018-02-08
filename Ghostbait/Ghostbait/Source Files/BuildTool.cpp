#include "BuildTool.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "Console.h"
#include "VRManager.h"


BuildTool::BuildTool() { 
	//std::vector<unsigned> prefabIDs;
	//prefabIDs.resize(3);
	//prefabIDs[0] = 2;
	//prefabIDs[1] = 2;
	//prefabIDs[2] = 2;
	//SetPrefabs(prefabIDs);
	state = BUILD;
}
BuildTool::BuildTool(std::vector<unsigned> prefabIDs) {
	SetPrefabs(prefabIDs);
	state = BUILD;
}

void BuildTool::SetPrefabs(std::vector<unsigned> prefabIDs) {
	prefabs.empty();
	prefabs.resize(prefabIDs.size() + 1);

	for (int i = 0; i < prefabIDs.size(); ++i) {
		prefabs[i] = BuildItem();
		prefabs[i].ID = prefabIDs[i];
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabIDs[i], { 0, 0, 0 }, &prefabs[i].object));
		MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(prefabs[i].object));
		//Set objects shader to be semi-transparent solid color
	}
	//Add removal tool
}
void BuildTool::SetParent(ControllerObject* _parent) {
	parent = _parent;
}

void BuildTool::Projection() {
	if (currentMode == SPAWN) SpawnProjection();
	else RemoveProjection();
}

void BuildTool::Activate() {
	if (currentMode == SPAWN) Spawn();
	else Remove();
}

void BuildTool::SpawnProjection(){
	spawnPos = VRManager::GetInstance().ArcCast(parent);
	if(!VRManager::GetInstance().ArcCastMissed(spawnPos)) {
	//snap to center of grid
		if (prefabs[currentPrefabIndex].object) {
			prefabs[currentPrefabIndex].object->position._41 = spawnPos.x;
			prefabs[currentPrefabIndex].object->position._42 = spawnPos.y;
			prefabs[currentPrefabIndex].object->position._43 = spawnPos.z;
		}
	}
}
void BuildTool::Spawn() {
	//Instantiate a stationary copy at this position to stay
	//Only spawn if grid position is empty
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabs[currentPrefabIndex].ID, spawnPos));
}
void BuildTool::RemoveProjection() {
	//Ray cast to object
	currentlySelectedItem = nullptr; //Raycasted object
	//Set shader to transparent thing
}
void BuildTool::Remove() {
	if (currentlySelectedItem)
		MessageEvents::SendMessage(EVENT_Destroy, DestroyMessage(currentlySelectedItem));
}

void BuildTool::CycleForward() {
	if (prefabs[currentPrefabIndex].object)
		MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(prefabs[currentPrefabIndex].object));
	
	int tempIndex = ++currentPrefabIndex;

	if (tempIndex >= (int)prefabs.size()) {
		tempIndex = 0;
	}

	if (prefabs[tempIndex].ID < 0)
		currentMode = Mode::REMOVE;
	else
		currentMode = Mode::SPAWN;

	currentPrefabIndex = tempIndex;
	if (prefabs[currentPrefabIndex].object)
		MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(prefabs[currentPrefabIndex].object));
}
void BuildTool::CycleBackward() {
	if (prefabs[currentPrefabIndex].object)
		MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(prefabs[currentPrefabIndex].object));

	int tempIndex = --currentPrefabIndex;

	//if index is out of range, loop it
	if (tempIndex < 0) {
		tempIndex = (int)prefabs.size() - 1;
	}

	//if index is removal tool...
	if (prefabs[tempIndex].ID < 0)
		currentMode = Mode::REMOVE;
	else
		currentMode = Mode::SPAWN;

	currentPrefabIndex = tempIndex;
	if (prefabs[currentPrefabIndex].object)
		MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(prefabs[currentPrefabIndex].object));

}

void BuildTool::InactiveUpdate() {
}
void BuildTool::ActiveUpdate() {
}

