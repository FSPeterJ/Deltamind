#include "BuildTool.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "Console.h"
#include "VRManager.h"
#include "PhysicsComponent.h"
#include "PhysicsExtension.h"

BuildTool::BuildTool() { 
	state = BUILD;
}

void BuildTool::SetPrefabs(std::vector<unsigned> prefabIDs) {
	prefabs.empty();
	prefabs.resize(prefabIDs.size() + 1);

	for (int i = 0; i < prefabIDs.size(); ++i) {
		prefabs[i] = BuildItem();
		prefabs[i].ID = prefabIDs[i];
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabIDs[i], { 0, 0, 0 }, &prefabs[i].object));
		if(prefabs[i].ID) MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(prefabs[i].object));
		PhysicsComponent* physComp = prefabs[i].object->GetComponent<PhysicsComponent>();
		if(physComp) physComp->isActive = false;
		//Set objects shader to be semi-transparent solid color
	}
	//Add removal tool
	prefabs[prefabs.size() - 1] = BuildItem();
	prefabs[prefabs.size() - 1].ID = 0;
}

void BuildTool::Enable(bool onEndDestroy) {
}

void BuildTool::Disable() {
}

void BuildTool::Update() {
	Item::Update();
}

void BuildTool::Projection() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < prefabs.size()) {
		if (currentMode == SPAWN) SpawnProjection();
		else RemoveProjection();
	}
}

void BuildTool::Activate() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < prefabs.size()) {
		if (currentMode == SPAWN) Spawn();
		else Remove();
	}
}

bool BuildTool::Snap(GameObject** obj) {
	DirectX::XMFLOAT2 pos = { (*obj)->position._41, (*obj)->position._43 };
	if (Snap(&pos)) {
		(*obj)->position._41 = pos.x;
		(*obj)->position._43 = pos.y;
		return true;
	}
	return false;
}
bool BuildTool::Snap(DirectX::XMFLOAT2* pos) {
	bool occupied;
	MessageEvents::SendMessage(EVENT_SnapRequest, SnapMessage(pos, &occupied));
	return occupied;
}
bool BuildTool::SetObstacle(DirectX::XMFLOAT2 pos, bool active) {
	bool success;
	if(active)MessageEvents::SendMessage(EVENT_AddObstacle, SnapMessage(&pos, &success));
	else MessageEvents::SendMessage(EVENT_RemoveObstacle, SnapMessage(&pos, &success));
	return success;
}

void BuildTool::SpawnProjection(){
	if (VRManager::GetInstance().ArcCast((Object*)this, &spawnPos)) {
		//snap to center of grid
		DirectX::XMFLOAT2 newPos = DirectX::XMFLOAT2(spawnPos.x, spawnPos.z);
		Snap(&newPos);
		spawnPos.x = newPos.x;
		spawnPos.z = newPos.y;
		//Move Object
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
	DirectX::XMFLOAT2 pos = DirectX::XMFLOAT2(spawnPos.x, spawnPos.z);
	if (Snap(&pos)) {
		if (SetObstacle(pos, true)) {
			spawnPos.x = pos.x;
			spawnPos.z = pos.y;
			GameObject* newObj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabs[currentPrefabIndex].ID, spawnPos, &newObj));
			builtItems.push_back(newObj);
			newObj->Enable();
			//Console::WriteLine << newObj << "was built at position (" << newObj->position._41 << ", " << newObj->position._42 << ", " << newObj->position._43 << ")!";
		}
	}
}
void BuildTool::RemoveProjection() {
	DirectX::XMFLOAT3 endPos;
	if (!Raycast(DirectX::XMFLOAT3(position._41, position._42, position._43), DirectX::XMFLOAT3(position._31, position._32, position._33), &endPos, &currentlySelectedItem, 4)) {
		currentlySelectedItem = nullptr;
	}
	//Set shader to transparent thing
}
void BuildTool::Remove() {
	for (int i = 0; i < builtItems.size(); ++i) {
		if (currentlySelectedItem == builtItems[i]) {
			DirectX::XMFLOAT2 pos = DirectX::XMFLOAT2(currentlySelectedItem->position._41, currentlySelectedItem->position._43);
			if (SetObstacle(pos, false)) {
				MessageEvents::SendQueueMessage(EVENT_Late, [=] { currentlySelectedItem->Destroy(); });
				builtItems.erase(builtItems.begin() + i);
				//Console::WriteLine << currentlySelectedItem << "was removed at position (" << currentlySelectedItem->position._41 << ", " << currentlySelectedItem->position._42 << ", " << currentlySelectedItem->position._43 << ")!";
			}
			break;
		}
	}
}

void BuildTool::CycleForward() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));

		int tempIndex = ++currentPrefabIndex;

		if (tempIndex >= (int)prefabs.size()) {
			tempIndex = 0;
		}

		if (prefabs[tempIndex].ID == 0)
			currentMode = Mode::REMOVE; // Unreachable code
		else
			currentMode = Mode::SPAWN;

		currentPrefabIndex = tempIndex;
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Addrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));
	}
}
void BuildTool::CycleBackward() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));

		int tempIndex = --currentPrefabIndex;

		//if index is out of range, loop it
		if (tempIndex < 0) {
			tempIndex = (int)prefabs.size() - 1;
		}

		//if index is removal tool...
		if (prefabs[tempIndex].ID == 0)
			currentMode = Mode::REMOVE;
		else
			currentMode = Mode::SPAWN;

		currentPrefabIndex = tempIndex;
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Addrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));
	}
}

void BuildTool::InactiveUpdate() {
}
void BuildTool::ActiveUpdate() {
}

void BuildTool::DeSelected() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));
	}
}

void BuildTool::Selected() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Addrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));
	}
}

