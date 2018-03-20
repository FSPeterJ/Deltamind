#include "BuildTool.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "Console.h"
#include "PhysicsComponent.h"
#include "PhysicsExtension.h"
#include "HexGrid.h"
#include "Material.h"
#include "Turret.h"

BuildTool::BuildTool() { 
	state = BUILD;
}

void BuildTool::SetPrefabs(std::vector<unsigned> prefabIDs) {
	prefabs.empty();
	prefabs.resize(prefabIDs.size() + 1);

	for (size_t i = 0; i < prefabIDs.size(); ++i) {
		prefabs[i] = BuildItem();
		prefabs[i].ID = prefabIDs[i];
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabIDs[i], { 0, 0, 0 }, &prefabs[i].object));
		if(prefabs[i].ID) MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(prefabs[i].object));
		PhysicsComponent* physComp = prefabs[i].object->GetComponent<PhysicsComponent>();
		prefabs[i].object->PersistOnReset();
		if(physComp) physComp->isActive = false;
		//Set objects shader to be semi-transparent solid color
		if (prefabs[currentPrefabIndex].object->SwapComponentVarient<Material>("invalid")) {
			prevLocationValid = false;
		}
	}
	//Add removal tool
	prefabs[prefabs.size() - 1] = BuildItem();
	prefabs[prefabs.size() - 1].ID = 0;
}

void BuildTool::Enable() {
}

void BuildTool::Disable() {
}

void BuildTool::Update() {
	Item::Update();
}

void BuildTool::Projection() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (currentMode == SPAWN) SpawnProjection();
		else RemoveProjection();
	}
}

void BuildTool::Activate() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (currentMode == SPAWN) Spawn();
		else Remove();
	}
}

bool BuildTool::Snap(GameObject** obj) {
	DirectX::XMFLOAT2 pos = { (*obj)->transform.GetMatrix()._41, (*obj)->transform.GetMatrix()._43 };
	if (Snap(&pos)) {
		DirectX::XMFLOAT4X4 newPos = (*obj)->transform.GetMatrix();
		newPos._41 = pos.x;
		newPos._43 = pos.y;
		(*obj)->transform.SetMatrix(newPos);
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
	if (grid->IsBlocked(pos) == active) {
		return false;
	}
	if (active) grid->AddObstacle(pos);
	if (!active) grid->RemoveObstacle(pos);
	return true;
}

void BuildTool::SpawnProjection(){
	if (prefabs[currentPrefabIndex].object) {
		if (ArcCast(&transform, &spawnPos, &buildArc)) {
			//snap to center of grid
			buildArc.Create();
			DirectX::XMFLOAT2 newPos = DirectX::XMFLOAT2(spawnPos.x, spawnPos.z);
			Snap(&newPos);
			spawnPos.x = newPos.x;
			spawnPos.z = newPos.y;

			//Move Object
			DirectX::XMFLOAT4X4 newPos1 = prefabs[currentPrefabIndex].object->transform.GetMatrix();
			newPos1._41 = spawnPos.x;
			newPos1._42 = spawnPos.y;
			newPos1._43 = spawnPos.z;
			prefabs[currentPrefabIndex].object->transform.SetMatrix(newPos1);

			Turret* turret = dynamic_cast<Turret*>(prefabs[currentPrefabIndex].object);
			bool hasEnoughMoney = true;
			if (turret) hasEnoughMoney = *gears >= turret->GetBuildCost(); //Disable for testing
			bool maxTurretsSpawned = (*maxTurrets - *turretsSpawned) <= 0;

			if ((grid->IsBlocked(newPos.x, newPos.y) || !hasEnoughMoney || maxTurretsSpawned) && prevLocationValid) {
				if (prefabs[currentPrefabIndex].object->SwapComponentVarient<Material>("invalid")) {
					prevLocationValid = false;
				}
			}
			else if(!grid->IsBlocked(newPos.x, newPos.y) && !prevLocationValid && hasEnoughMoney && !maxTurretsSpawned) {
				if (prefabs[currentPrefabIndex].object->SwapComponentVarient<Material>("valid")) {
					prevLocationValid = true;
				}
			}
		}
		else {
			buildArc.Destroy();
		}
	}

}
void BuildTool::Spawn() {
	//Instantiate a stationary copy at this position to stay
	//Only spawn if grid position is empty
	Turret* turret = dynamic_cast<Turret*>(prefabs[currentPrefabIndex].object);
	bool hasEnoughMoney = true;
	if (turret) hasEnoughMoney = *gears >= turret->GetBuildCost();
	bool maxTurretsSpawned = (*maxTurrets - *turretsSpawned) <= 0;

	DirectX::XMFLOAT2 pos = DirectX::XMFLOAT2(spawnPos.x, spawnPos.z);
	if (Snap(&pos) && hasEnoughMoney && !maxTurretsSpawned) {
		if (SetObstacle(pos, true)) {
			*gears -= turret->GetBuildCost();
			spawnPos.x = pos.x;
			spawnPos.z = pos.y;
			GameObject* newObj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabs[currentPrefabIndex].ID, spawnPos, &newObj));
			builtItems.push_back(newObj);
			newObj->Enable();
			(*turretsSpawned) = (*turretsSpawned) + 1;
		}
	}
}
void BuildTool::RemoveProjection() {
	DirectX::XMFLOAT3 endPos;
	GameObject* colObject = nullptr;
	if(!Raycast(&transform, DirectX::XMFLOAT3(transform.GetMatrix()._31, transform.GetMatrix()._32, transform.GetMatrix()._33), &endPos, &colObject, &deleteRay, 4)) {
		if (currentlySelectedItem) {
			currentlySelectedItem->SwapComponentVarient<Material>("default");
			currentlySelectedItemIndex = -1;
			currentlySelectedItem = nullptr;
		}
		return;
	}
	//check if I spawned it
	if (colObject != currentlySelectedItem && currentlySelectedItem) {
		currentlySelectedItem->SwapComponentVarient<Material>("default");
	}
	for (size_t i = 0; i < builtItems.size(); ++i) {
		if (colObject == builtItems[i]) {
			currentlySelectedItemIndex = (int)i;
			currentlySelectedItem = colObject;
			//TODO: Temp...Dont't call this every frame either.
			currentlySelectedItem->SwapComponentVarient<Material>("invalid");
			break;
		}
	}
}
void BuildTool::Remove() {
	if (currentlySelectedItem) {
		DirectX::XMFLOAT2 pos = DirectX::XMFLOAT2(currentlySelectedItem->transform.GetMatrix()._41, currentlySelectedItem->transform.GetMatrix()._43);
		if (SetObstacle(pos, false)) {
			MessageEvents::SendQueueMessage(EVENT_Late, [=] { currentlySelectedItem->Destroy(); });
			builtItems.erase(builtItems.begin() + currentlySelectedItemIndex);
			(*turretsSpawned) = (*turretsSpawned) - 1;
		}
	}
}

void BuildTool::CycleForward() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));

		int tempIndex = ++currentPrefabIndex;

		if (tempIndex >= (int)prefabs.size()) {
			tempIndex = 0;
		}

		if (prefabs[tempIndex].ID == 0) {
			currentMode = Mode::REMOVE;
			deleteRay.Create();
		}
		else {
			currentMode = Mode::SPAWN;
			if (currentlySelectedItem) {
				currentlySelectedItem->SwapComponentVarient<Material>("default");
				currentlySelectedItem = nullptr;
			}
			deleteRay.Destroy();
		}

		currentPrefabIndex = tempIndex;
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Addrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));
	}
}
void BuildTool::CycleBackward() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));

		int tempIndex = --currentPrefabIndex;

		//if index is out of range, loop it
		if (tempIndex < 0) {
			tempIndex = (int)prefabs.size() - 1;
		}

		//if index is removal tool...
		if (prefabs[tempIndex].ID == 0) {
			currentMode = Mode::REMOVE;
			deleteRay.Create();
		}
		else {
			currentMode = Mode::SPAWN;
			if (currentlySelectedItem) {
				currentlySelectedItem->SwapComponentVarient<Material>("default");
				currentlySelectedItem = nullptr;
			}
			deleteRay.Destroy();
		}

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
	Item::DeSelected();
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));
	}
	if (currentlySelectedItem) {
		currentlySelectedItem->SwapComponentVarient<Material>("default");
		currentlySelectedItem = nullptr;
	}
	buildArc.Destroy();
	deleteRay.Destroy();
}

void BuildTool::Selected() {
	Item::Selected();
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			MessageEvents::SendMessage(EVENT_Addrender, StandardObjectMessage(prefabs[currentPrefabIndex].object));
		if (prefabs[currentPrefabIndex].ID == 0) {
			deleteRay.Create();
		}
	}
}

void BuildTool::Awake(Object* obj) {
	buildArc.SetFile("Assets/Arc2.ghost");
	deleteRay.SetFile("Assets/Ray.ghost");
	GameObject::Awake(obj);
}

