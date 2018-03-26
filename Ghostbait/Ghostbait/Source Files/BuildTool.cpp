#include "BuildTool.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "Console.h"
#include "PhysicsComponent.h"
#include "PhysicsExtension.h"
#include "HexGrid.h"
#include "Material.h"
#include "Turret.h"
#include "TextManager.h"
#include "ObjectFactory.h"
#include "DebugRenderer.h"

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
		if (prefabs[i].ID) prefabs[i].object->UnRender();
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

void BuildTool::ActiveUpdate() {
	if (!gearDisplay) {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/CurrencyQuad.ghost")), { 0, 0, 0 }, &gearDisplay));
		gearDisplay->SetComponent<Material>(TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "$5000000", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 0.5f)).mat);
		gearDisplay->PersistOnReset();
		gearDisplay->ToggleFlag(GAMEOBJECT_PUBLIC_FLAGS::UNLIT);
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/CurrencyQuadSmall.ghost")), { 0, 0, 0 }, &gearAdjustmentDisplay));
		gearAdjustmentDisplay->SetComponent<Material>(TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "-$5000000", DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)).mat);
		gearAdjustmentDisplay->PersistOnReset();
		gearAdjustmentDisplay->ToggleFlag(GAMEOBJECT_PUBLIC_FLAGS::UNLIT);
	}

	if (gearDisplay) {
		//Main
		std::string text = "$";
		text.append(std::to_string(*gears));
		for (int i = (int)text.length(); i < 6; ++i)
			text.insert(0, " ");
		TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", text, gearDisplay->GetComponent<Material>(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 0.5f));
		DirectX::XMFLOAT4X4 displayMat = transform.GetMatrix();
		displayMat._41 -= (displayMat._11 * 0.15f);
		displayMat._42 -= (displayMat._12 * 0.15f);
		displayMat._43 -= (displayMat._13 * 0.15f);
		gearDisplay->transform.SetMatrix(displayMat);
		//Adjustment
		text = "$";
		if (currentMode == SPAWN) {
			text.insert(0, "-");
			text.append(std::to_string(((Turret*)prefabs[currentPrefabIndex].object)->GetBuildCost()));
			for (int i = (int)text.length(); i < 14; ++i)
				text.insert(0, " ");
			text.append("\n");
			TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", text, gearAdjustmentDisplay->GetComponent<Material>(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 0.5f));
			DirectX::XMFLOAT4X4 adjustmentMat = displayMat;
			adjustmentMat._41 -= (adjustmentMat._21 * 0.1f);
			adjustmentMat._42 -= (adjustmentMat._22 * 0.1f);
			adjustmentMat._43 -= (adjustmentMat._23 * 0.1f);
			gearAdjustmentDisplay->transform.SetMatrix(adjustmentMat);
		}
		else if(currentlySelectedItem){
				text.insert(0, "+");
				text.append(std::to_string((int)(((Turret*)currentlySelectedItem)->GetBuildCost() * 0.5f)));
				for (int i = (int)text.length(); i < 14; ++i)
					text.insert(0, " ");
				text.append("\n");
				TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", text, gearAdjustmentDisplay->GetComponent<Material>(), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 0.5f));
				DirectX::XMFLOAT4X4 adjustmentMat = displayMat;
				adjustmentMat._41 -= (adjustmentMat._21 * 0.1f);
				adjustmentMat._42 -= (adjustmentMat._22 * 0.1f);
				adjustmentMat._43 -= (adjustmentMat._23 * 0.1f);
				gearAdjustmentDisplay->transform.SetMatrix(adjustmentMat);
		}
	}
	Item::ActiveUpdate();
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

bool BuildTool::CanBuildHere(DirectX::XMFLOAT2& spawnPos) {
	Turret* turret = dynamic_cast<Turret*>(prefabs[currentPrefabIndex].object);
	bool hasEnoughMoney = true;
	if (turret) hasEnoughMoney = *gears >= turret->GetBuildCost();
	bool maxTurretsSpawned = (*maxTurrets - *turretsSpawned) <= 0;
	bool isBlocked = grid->IsBlocked(spawnPos);
	bool isValidTile = grid->PointToTile(spawnPos) != nullptr;
	if (isValidTile) {
		isValidTile = !isBlocked;
	}

	if (isValidTile && hasEnoughMoney && !maxTurretsSpawned)
		return true;
	else
		return false;
}

bool BuildTool::Snap(GameObject** obj) {
	DirectX::XMFLOAT2 pos = { (*obj)->transform.GetMatrix()._41, (*obj)->transform.GetMatrix()._43 };
	DirectX::XMFLOAT2 snappedPoint;
	if (grid->Snap(pos, snappedPoint)) {
		DirectX::XMFLOAT4X4 newPos = (*obj)->transform.GetMatrix();
		newPos._41 = snappedPoint.x;
		newPos._43 = snappedPoint.y;
		(*obj)->transform.SetMatrix(newPos);
		return true;
	}
	return false;
}
bool BuildTool::Snap(DirectX::XMFLOAT2* pos) {
	DirectX::XMFLOAT2 snappedPoint;
	if (grid->Snap(*pos, snappedPoint)) {
		*pos = snappedPoint;
		return true;
	}
	return false;
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
			
			//Asses if valid location
			if (CanBuildHere(newPos)) {
				if (!prevLocationValid) {
					prefabs[currentPrefabIndex].object->SwapComponentVarient<Material>("valid");
					prevLocationValid = true;
				}
				gearAdjustmentDisplay->RenderTransparent();
			}
			else {
				if (prevLocationValid) {
					prefabs[currentPrefabIndex].object->SwapComponentVarient<Material>("invalid");
					prevLocationValid = false;
				}
				gearAdjustmentDisplay->UnRender();
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
		gearAdjustmentDisplay->UnRender();
		return;
	}

	//check if I spawned it
	if (colObject != currentlySelectedItem) {
		if(currentlySelectedItem) currentlySelectedItem->SwapComponentVarient<Material>("default");
		gearAdjustmentDisplay->UnRender();
	}
	for (size_t i = 0; i < builtItems.size(); ++i) {
		if (colObject == builtItems[i]) {
			currentlySelectedItemIndex = (int)i;
			currentlySelectedItem = colObject;
			//TODO: Temp...Dont't call this every frame.
			currentlySelectedItem->SwapComponentVarient<Material>("invalid");
			gearAdjustmentDisplay->RenderTransparent();
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
			Turret* tur = dynamic_cast<Turret*>(currentlySelectedItem);
			if (tur) {
				(*gears) = (*gears) + (int)(tur->GetBuildCost() * 0.5f);
				gearAdjustmentDisplay->UnRender();
			}
			(*turretsSpawned) = (*turretsSpawned) - 1;
		}
	}
}

void BuildTool::CycleForward() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (prefabs[currentPrefabIndex].object) {
			prefabs[currentPrefabIndex].object->UnRender();
			if(gearDisplay) gearDisplay->UnRender();
		}

		int tempIndex = ++currentPrefabIndex;

		if (tempIndex >= (int)prefabs.size()) {
			tempIndex = 0;
		}

		if (prefabs[tempIndex].ID == 0) {
			currentMode = Mode::REMOVE;
			buildArc.Destroy();
			deleteRay.Create();
		}
		else {
			currentMode = Mode::SPAWN;
			if (currentlySelectedItem) {
				currentlySelectedItem->SwapComponentVarient<Material>("default");
				currentlySelectedItem = nullptr;
			}
			buildArc.Create();
			deleteRay.Destroy();
		}

		currentPrefabIndex = tempIndex;
		if (prefabs[currentPrefabIndex].object)
			prefabs[currentPrefabIndex].object->Render();
	}
}
void BuildTool::CycleBackward() {
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (prefabs[currentPrefabIndex].object) {
			prefabs[currentPrefabIndex].object->UnRender();
			if (gearDisplay) gearDisplay->UnRender();
		}

		int tempIndex = --currentPrefabIndex;

		//if index is out of range, loop it
		if (tempIndex < 0) {
			tempIndex = (int)prefabs.size() - 1;
		}

		//if index is removal tool...
		if (prefabs[tempIndex].ID == 0) {
			currentMode = Mode::REMOVE;
			buildArc.Destroy();
			deleteRay.Create();
		}
		else {
			currentMode = Mode::SPAWN;
			if (currentlySelectedItem) {
				currentlySelectedItem->SwapComponentVarient<Material>("default");
				currentlySelectedItem = nullptr;
			}
			buildArc.Create();
			deleteRay.Destroy();
		}

		currentPrefabIndex = tempIndex;
		if (prefabs[currentPrefabIndex].object)
			prefabs[currentPrefabIndex].object->Render();
	}
}

void BuildTool::InactiveUpdate() {
}

void BuildTool::DeSelected() {
	if(gearDisplay) gearDisplay->UnRender();
	if (gearAdjustmentDisplay) gearAdjustmentDisplay->UnRender();
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (prefabs[currentPrefabIndex].object)
			prefabs[currentPrefabIndex].object->UnRender();
	}
	if (currentlySelectedItem) {
		currentlySelectedItem->SwapComponentVarient<Material>("default");
		currentlySelectedItem = nullptr;
	}
	buildArc.Destroy();
	deleteRay.Destroy();
	Item::DeSelected();
}

void BuildTool::Selected() {
	Item::Selected();
	if (currentPrefabIndex >= 0 && currentPrefabIndex < (int)prefabs.size()) {
		if (gearDisplay) gearDisplay->RenderTransparent();
		if (gearAdjustmentDisplay) gearAdjustmentDisplay->RenderTransparent();
		if (prefabs[currentPrefabIndex].object)
			prefabs[currentPrefabIndex].object->Render();
		if (prefabs[currentPrefabIndex].ID == 0)
			deleteRay.Create();
	}
}

void BuildTool::Awake(Object* obj) {
	buildArc.SetFile("Assets/Arc2.ghost");
	deleteRay.SetFile("Assets/Ray.ghost");
	//gearDisplay->transform.SetMatrix(DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1));
	//gearDisplay->SetComponent<Material>(TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "$0").mat);
	//Material* newMat = TextManager::CreateRenderableTexture(100, 100);
	//TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", "This is a test!", newMat);
	//TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", "This is a test!", GetComponent<Material>());
	GameObject::Awake(obj);
}

