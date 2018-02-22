#include "ControllerObject.h"
#include "Console.h"         // for Console, Console::WriteLine, Console::WriteLiner
#include "MessageEvents.h"
#include "VRManager.h"
#include "BuildTool.h"
#include "PhysicsComponent.h"
#include "BuildTool.h"

ControllerObject::ControllerObject() {
	items.resize(4);
	displayItems.resize(4);
	hand = HAND_Invalid;
}

void ControllerObject::Init(ControllerHand _hand, int menuControllerPrefabID) {
	SetControllerHand(_hand);
	Enable();
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuControllerItem>(menuControllerPrefabID, { 0,0,0 }, &menuController));
	int temp = sizeof(MenuControllerItem);
}
void ControllerObject::SetPhysicsComponent(const GameObject* obj, bool active) {
	PhysicsComponent* physComp = obj->GetComponent<PhysicsComponent>();
	if (physComp) physComp->isActive = active;
}
void ControllerObject::AddToInventory(int itemSlot, char* prefabName) {
	//Actual Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<Gun>(prefabName, { 0,0,0 }, (Gun**)&items[itemSlot]));
	if (!currentGameItem) currentGameItem = items[itemSlot];
	else {
		items[itemSlot]->Render(false);
		SetPhysicsComponent(items[itemSlot], false);
	}
	
	//Inventory Display
	MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<Gun>(prefabName, { 0,0,0 }, (Gun**)&displayItems[itemSlot]));
	displayItems[itemSlot]->Render(false);
	SetPhysicsComponent(displayItems[itemSlot], false);
}

void ControllerObject::SwitchCurrentItem(int itemIndex) {
	if (itemIndex == -1) {
		Control item0 = (hand == HAND_Left ? leftItem0 : rightItem0);
		Control item1 = (hand == HAND_Left ? leftItem1 : rightItem1);
		Control item2 = (hand == HAND_Left ? leftItem2 : rightItem2);
		Control item3 = (hand == HAND_Left ? leftItem3 : rightItem3);
		if (KeyIsDown(item0)) {
			if (items[0]) {
				SwitchCurrentItem(0);
				ResetKey(item0);
				return;
			}
		}
		if (KeyIsDown(item1)) {
			if (items[1]) {
				SwitchCurrentItem(1);
				ResetKey(item1);
				return;
			}
		}
		if (KeyIsDown(item2)) {
			if (items[2]) {
				SwitchCurrentItem(2);
				ResetKey(item2);
				return;
			}
		}
		if (KeyIsDown(item3)) {
			if (items[3]) {
				SwitchCurrentItem(3);
				ResetKey(item3);
				return;
			}
		}
	}
	else {
		currentGameItem->Render(false);
		SetPhysicsComponent(currentGameItem, false);
		currentGameItem = items[itemIndex];
		currentGameItem->Render(true);
		SetPhysicsComponent(currentGameItem, true);
		return;
	}
}
void ControllerObject::DisplayInventory() {
	static bool leftJustTouched = false, rightJustTouched = false;

	bool* justTouched = (hand == HAND_Left ? &leftJustTouched : &rightJustTouched);
	Control touch = (hand == HAND_Left ? leftTouch : rightTouch);

	if (KeyIsDown(touch)) {
		for (unsigned int i = 0; i < displayItems.size(); ++i) {
			if (displayItems[i]) {
				if (!*justTouched) displayItems[i]->Render(true);

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
		*justTouched = true;
	}
	else {
		if (*justTouched) {
			for (unsigned int i = 0; i < displayItems.size(); ++i) {
				if (displayItems[i]) displayItems[i]->Render(false);
			}
			*justTouched = false;
		}
	}
}

void ControllerObject::AddItem(int itemSlot, char* prefabName) {
	AddToInventory(itemSlot, prefabName);

	Gun* gun = dynamic_cast<Gun*>(items[itemSlot]);
	BuildTool* tool = dynamic_cast<BuildTool*>(items[itemSlot]);
	if (gun) {
		gun->Init();
		gun->SetStats(Gun::FireType::SEMI, 60, 1);
	}
}
void ControllerObject::AddItem(int itemSlot, char* prefabName, std::vector<int> prefabIDs) {
	AddToInventory(itemSlot, prefabName);

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
void ControllerObject::AddItem(int itemSlot, char* prefabName, Gun::FireType _fireType, float _fireRate, float _damage) {
	AddToInventory(itemSlot, prefabName);

	Gun* gun = dynamic_cast<Gun*>(items[itemSlot]);
	if (gun) {
		gun->Init();
		gun->SetStats(_fireType, _fireRate, _damage);
	}
}

void ControllerObject::Update() {
	if(hand == HAND_Invalid) return;
	menuController->Render(false);
	
	//Seperate controller Values
	Control attack = (hand == HAND_Left ? leftAttack : rightAttack);
	Control cyclePrefab = (hand == HAND_Left ? leftCyclePrefab : rightCyclePrefab);

	//Handle Inventory
	SwitchCurrentItem();
	DisplayInventory();

	//Update Items
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

			//Specific States
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
					//static bool leftCycled = false, rightCycled = false;
					if (/*!(hand == HAND_Left ? leftCycled : rightCycled) && */KeyIsDown(cyclePrefab)) {
						((BuildTool*)currentGameItem)->CycleForward();
						ResetKey(cyclePrefab);
						//if(hand == HAND_Left) leftCycled = true;
						//else if (hand == HAND_Right) rightCycled = true;
					}
					//if (!KeyIsDown(cyclePrefab)) {
					//	if (hand == HAND_Left) leftCycled = false;
					//	else if (hand == HAND_Right) rightCycled = false;
					//}

					//static bool leftAttacked = false, rightAttacked = false;
					if (/*!(hand == HAND_Left ? leftAttacked : rightAttacked) && */KeyIsDown(attack)) {
						((BuildTool*)currentGameItem)->Activate();
						//if (hand == HAND_Left) leftAttacked = true;
						//else if (hand == HAND_Right) rightAttacked = true;
						ResetKey(attack);
					}
					if (!KeyIsDown(attack)) {
						((BuildTool*)currentGameItem)->Projection();
					//	if (hand == HAND_Left) leftAttacked = false;
					//	else if (hand == HAND_Right) rightAttacked = false;
					}
				}
				break;
			case Item::State::INVALID:
				break;
			}

			static bool teleport = false;
			//All states
			if (KeyIsDown(teleportDown) && hand == HAND_Right) {
				VRManager::GetInstance().ArcCast(this, {});
				teleport = true;
			}
			if (teleport && !KeyIsDown(teleportDown) && hand == HAND_Right) {
				VRManager::GetInstance().Teleport();
				teleport = false;
			}
		}
	#pragma endregion
}
void ControllerObject::PausedUpdate() {
	if (hand == HAND_Invalid) return;
	Control attack = (hand == HAND_Left ? leftAttack : rightAttack);
	
	menuController->Render(true);
	menuController->position = position;
	
	if (KeyIsDown(attack)) {
		menuController->Activate();
		//ResetKey(attack);
	}
	else menuController->UpdateRay();
}
