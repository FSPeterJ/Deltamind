#include "BuildTool.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "Console.h"
#include "VRManager.h"
#include "PhysicsComponent.h"
#include "PhysicsExtension.h"

BuildTool::BuildTool() { 
	//std::vector<unsigned> prefabIDs;
	//prefabIDs.resize(3);
	//prefabIDs[0] = 2;
	//prefabIDs[1] = 2;
	//prefabIDs[2] = 2;
	//SetPrefabs(prefabIDs);
	state = BUILD;
}
BuildTool::BuildTool(std::vector<int> prefabIDs) {
	SetPrefabs(prefabIDs);
	state = BUILD;
}

void BuildTool::SetPrefabs(std::vector<int> prefabIDs) {
	prefabs.empty();
	prefabs.resize(prefabIDs.size() + 1);

	for (int i = 0; i < prefabIDs.size(); ++i) {
		prefabs[i] = BuildItem();
		prefabs[i].ID = prefabIDs[i];
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabIDs[i], { 0, 0, 0 }, &prefabs[i].object));
		if(i) MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(prefabs[i].object));
		PhysicsComponent* physComp = prefabs[i].object->GetComponent<PhysicsComponent>();
		if(physComp) physComp->isActive = false;
		//Set objects shader to be semi-transparent solid color
	}
	//Add removal tool
	prefabs[prefabs.size() - 1] = BuildItem();
	prefabs[prefabs.size() - 1].ID = -1;
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
	if(VRManager::GetInstance().ArcCast(parent, &spawnPos)) {
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
	GameObject* newObj;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabs[currentPrefabIndex].ID, spawnPos, &newObj));
	builtItems.push_back(newObj);
}
void BuildTool::RemoveProjection() {
	DirectX::XMFLOAT3 endPos;
	Raycast(DirectX::XMFLOAT3(position._41, position._42, position._43), DirectX::XMFLOAT3(position._31, position._32, position._33), &endPos, (GameObject**)&currentlySelectedItem, 4);
	//Set shader to transparent thing
}
void BuildTool::Remove() {
	for (int i = 0; i < builtItems.size(); ++i) {
		if (currentlySelectedItem == builtItems[i]) {
			MessageEvents::SendQueueMessage(EVENT_Late, [=] { currentlySelectedItem->Destroy(); });
			builtItems.erase(builtItems.begin() + i);
			break;
		}
	}
}

void BuildTool::CycleForward() {
	if (prefabs[currentPrefabIndex].object)
		MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(prefabs[currentPrefabIndex].object));
	
	int tempIndex = ++currentPrefabIndex;

	if (tempIndex >= (int)prefabs.size()) {
		tempIndex = 0;
	}

	if (prefabs[tempIndex].ID < 0)
		currentMode = Mode::REMOVE; // Unreachable code
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

