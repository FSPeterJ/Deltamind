#include "ControllerObject.h"
#include "Console.h"         // for Console, Console::WriteLine, Console::WriteLiner
#include "MessageEvents.h"
#include "VRManager.h"
#include "BuildTool.h"

ControllerObject::ControllerObject() {
	items.resize(4);
	displayItems.resize(4);
	hand = INVALID;
}

void ControllerObject::AddItem(int itemSlot, int prefabID) {
	//Add Item to Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&items[itemSlot]));
	if (!currentGameItem) currentGameItem = items[itemSlot];
	else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));

	//Add Item to display Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&displayItems[itemSlot]));
	MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(displayItems[itemSlot]));

	Gun* gun = dynamic_cast<Gun*>(items[itemSlot]);
	if (gun) {
		gun->Init();
		gun->SetStats(Gun::FireType::SEMI, 60, 1);
	}

}
void ControllerObject::AddItem(int itemSlot, int prefabID, std::vector<unsigned> prefabIDs) {
	//Add Item to Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&items[itemSlot]));
	if (!currentGameItem) currentGameItem = items[itemSlot];
	else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));

	//Add Item to display Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&displayItems[itemSlot]));
	MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(displayItems[itemSlot]));

	Gun* gun = dynamic_cast<Gun*>(items[itemSlot]);
	BuildTool* buildTool = dynamic_cast<BuildTool*>(items[itemSlot]);
	if (gun) {
		gun->Init();
		gun->SetStats(Gun::FireType::SEMI, 60, 1);
	}
	else if (buildTool) {
		buildTool->SetPrefabs(prefabIDs);
		buildTool->SetParent(this);
	}
}
void ControllerObject::AddItem(int itemSlot, int prefabID, Gun::FireType _fireType, float _fireRate, float _damage) {
	//Add Item to Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&items[itemSlot]));
	if (!currentGameItem) currentGameItem = items[itemSlot];
	else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));

	//Add Item to display Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&displayItems[itemSlot]));
	MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(displayItems[itemSlot]));

	Gun* gun = dynamic_cast<Gun*>(items[itemSlot]);
	if (gun) {
		gun->Init();
		gun->SetStats(_fireType, _fireRate, _damage);
	}
}

void ControllerObject::Update() {
	if(hand == INVALID) return;
	
	//Seperate controller Values
	Control item1  = (hand == LEFT ? leftItem1 : rightItem1);
	Control item2  = (hand == LEFT ? leftItem2 : rightItem2);
	Control item3  = (hand == LEFT ? leftItem3 : rightItem3);
	Control item4  = (hand == LEFT ? leftItem4 : rightItem4);
	Control touch  = (hand == LEFT ? leftTouch : rightTouch);
	Control attack = (hand == LEFT ? leftAttack : rightAttack);
	Control cyclePrefab = (hand == LEFT ? leftCyclePrefab : rightCyclePrefab);

	#pragma region Switch Controller Item
		if (KeyIsDown(item1)) {
			if (items[0]) {
				MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentGameItem));
				currentGameItem = items[0];
				MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentGameItem));
			}
			ResetKey(item1);
		}
		else if (KeyIsDown(item2)) {
			if (items[1]) {
				MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentGameItem));
				currentGameItem = items[1];
				MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentGameItem));
			}
			ResetKey(item2);
		}
		else if (KeyIsDown(item3)) {
			if (items[2]) {
				MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentGameItem));
				currentGameItem = items[2];
				MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentGameItem));
			}
			ResetKey(item3);
		}
		else if (KeyIsDown(item4)) {
			if (items[3]) {
				MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentGameItem));
				currentGameItem = items[3];
				MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentGameItem));
			}
			ResetKey(item4);
		}
	#pragma endregion
	#pragma region Display Inventory
		if (KeyIsDown(touch)) {
			for (unsigned int i = 0; i < displayItems.size(); ++i) {
				if (displayItems[i]) {
					if (!touchHeld) {
						MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(displayItems[i]));
					}
					displayItems[i]->position._11 = position._11 * 0.5f;
					displayItems[i]->position._12 = position._12 * 0.5f;
					displayItems[i]->position._13 = position._13 * 0.5f;
					displayItems[i]->position._14 = position._14;
					displayItems[i]->position._21 = position._21 * 0.5f;
					displayItems[i]->position._22 = position._22 * 0.5f;
					displayItems[i]->position._23 = position._23 * 0.5f;
					displayItems[i]->position._24 = position._24;
					displayItems[i]->position._31 = position._31 * 0.5f;
					displayItems[i]->position._32 = position._32 * 0.5f;
					displayItems[i]->position._33 = position._33 * 0.5f;
					displayItems[i]->position._34 = position._34;
					displayItems[i]->position._41 = position._41;
					displayItems[i]->position._42 = position._42;
					displayItems[i]->position._43 = position._43;
					displayItems[i]->position._44 = position._44;
					switch (i) {
					case 0:
						displayItems[i]->position._41 += ((position._21 * 0.2f) + (position._31 * 0.1f));
						displayItems[i]->position._42 += ((position._22 * 0.2f) + (position._32 * 0.1f));
						displayItems[i]->position._43 += ((position._23 * 0.2f) + (position._33 * 0.1f));
						break;
					case 1:
						displayItems[i]->position._41 += ((-position._11 * 0.2f) + (position._31 * 0.1f));
						displayItems[i]->position._42 += ((-position._12 * 0.2f) + (position._32 * 0.1f));
						displayItems[i]->position._43 += ((-position._13 * 0.2f) + (position._33 * 0.1f));
						break;
					case 2:
						displayItems[i]->position._41 += ((position._11 * 0.2f) + (position._31 * 0.1f));
						displayItems[i]->position._42 += ((position._12 * 0.2f) + (position._32 * 0.1f));
						displayItems[i]->position._43 += ((position._13 * 0.2f) + (position._33 * 0.1f));
						break;
					case 3:
						displayItems[i]->position._41 += ((-position._21 * 0.2f) + (position._31 * 0.1f));
						displayItems[i]->position._42 += ((-position._22 * 0.2f) + (position._32 * 0.1f));
						displayItems[i]->position._43 += ((-position._23 * 0.2f) + (position._33 * 0.1f));
						break;
					}
				}
			}
			touchHeld = true;
		}
		else {
			for (unsigned int i = 0; i < displayItems.size(); ++i) {
				if (displayItems[i])
					MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(displayItems[i]));
			}
			touchHeld = false;
		}
	#pragma endregion
	#pragma region Update Inactive Items
		for (unsigned int i = 0; i < items.size(); ++i) {
			if (items[i] && items[i] != currentGameItem) {
				items[i]->InactiveUpdate();
			}
		}
	#pragma endregion
	#pragma region Update Current Item
		if (currentGameItem) {
			currentGameItem->position = position;
			currentGameItem->ActiveUpdate();
			switch (currentGameItem->state) {
			case Item::State::GUN:
				{
					if (KeyIsDown(attack)) {
						if (!((Gun*)currentGameItem)->Shoot())
							ResetKey(attack);
					}
				}
				break;
			case Item::State::CONTROLLER:
				{
				}
				break;
			case Item::State::BUILD:
				{
					if (KeyIsDown(cyclePrefab)) {
						ResetKey(cyclePrefab);
						((BuildTool*)currentGameItem)->CycleForward();
					}

					if (KeyIsDown(attack)) {
						((BuildTool*)currentGameItem)->Activate();
						ResetKey(attack);
					}
					else ((BuildTool*)currentGameItem)->Projection();
				}
				break;
			case Item::State::INVALID:
				break;
			}
			//All states
			if (KeyIsDown(teleportDown)) {
				VRManager::GetInstance().ArcCast(this);
			}
			if (KeyIsDown(teleportUp)) {
				ResetKey(teleportUp);
				ResetKey(teleportDown);
				VRManager::GetInstance().Teleport();
			}
		}
	#pragma endregion
}
