#include "ControllerObject.h"
#include "Console.h"         // for Console, Console::WriteLine, Console::WriteLiner
#include "MessageEvents.h"
#include "BuildTool.h"
#include "PhysicsComponent.h"
#include "BuildTool.h"
#include "Item.h"
#include "GhostTime.h"
#include "PhysicsExtension.h"
#include "Player.h"

ControllerObject::ControllerObject() {
}

void ControllerObject::Init(Player* _player, ControllerHand _hand) {
	player = _player;
	hand = _hand;
	//SetControllerHand(_hand); //Not needed anymore?
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuControllerItem>({ 0,0,0 }, &menuController));
	int temp = sizeof(MenuControllerItem);
	currentGameItem = items[0];
	Enable(false);
}
void ControllerObject::SetPhysicsComponent(const GameObject* obj, bool active) {
	PhysicsComponent* physComp = obj->GetComponent<PhysicsComponent>();
	if(physComp) physComp->isActive = active;
}
void ControllerObject::AddToInventory(int itemSlot, unsigned prefabID) {
	//Actual Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Item>(prefabID, { 0,0,0 }, (Item**)&items[itemSlot]));
	if(!currentGameItem) {
		currentGameItem = items[itemSlot];
		//currentGameItem->Selected();
	}
	else {
		items[itemSlot]->Render(false);
		SetPhysicsComponent(items[itemSlot], false);
	}

	//Inventory Display
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Item>(prefabID, { 0,0,0 }, (Item**)&displayItems[itemSlot]));
	displayItems[itemSlot]->Render(false);
	SetPhysicsComponent(displayItems[itemSlot], false);
}

void ControllerObject::SwitchCurrentItem(int itemIndex) {
	if(itemIndex == -1) {
		Control item0 = (hand == HAND_Left ? leftItem0 : rightItem0);
		Control item1 = (hand == HAND_Left ? leftItem1 : rightItem1);
		Control item2 = (hand == HAND_Left ? leftItem2 : rightItem2);
		Control item3 = (hand == HAND_Left ? leftItem3 : rightItem3);
		if(KeyIsDown(item0)) {
			if(items[0]) {
				SwitchCurrentItem(0);
				ResetKey(item0);
				return;
			}
		}
		if(KeyIsDown(item1)) {
			if(items[1]) {
				SwitchCurrentItem(1);
				ResetKey(item1);
				return;
			}
		}
		if(KeyIsDown(item2)) {
			if(items[2]) {
				SwitchCurrentItem(2);
				ResetKey(item2);
				return;
			}
		}
		if(KeyIsDown(item3)) {
			if(items[3]) {
				SwitchCurrentItem(3);
				ResetKey(item3);
				return;
			}
		}
	}
	else {
		currentGameItem->DeSelected();
		currentGameItem->Render(false);
		SetPhysicsComponent(currentGameItem, false);
		currentGameItem = items[itemIndex];
		currentGameItem->Selected();
		currentGameItem->Render(true);
		SetPhysicsComponent(currentGameItem, true);
		return;
	}
}
void ControllerObject::DisplayInventory() {
	static bool leftJustTouched = false, rightJustTouched = false;

	bool* justTouched = (hand == HAND_Left ? &leftJustTouched : &rightJustTouched);
	Control touch = (hand == HAND_Left ? leftTouch : rightTouch);

	if(KeyIsDown(touch)) {
		for(unsigned int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
			if(displayItems[i]) {
				if(!*justTouched) displayItems[i]->Render(true);
				DirectX::XMFLOAT4X4 newPos;
				newPos._11 = transform.GetMatrix()._11 * 0.5f;
				newPos._12 = transform.GetMatrix()._12 * 0.5f;
				newPos._13 = transform.GetMatrix()._13 * 0.5f;
				newPos._14 = transform.GetMatrix()._14;
				newPos._21 = transform.GetMatrix()._21 * 0.5f;
				newPos._22 = transform.GetMatrix()._22 * 0.5f;
				newPos._23 = transform.GetMatrix()._23 * 0.5f;
				newPos._24 = transform.GetMatrix()._24;
				newPos._31 = transform.GetMatrix()._31 * 0.5f;
				newPos._32 = transform.GetMatrix()._32 * 0.5f;
				newPos._33 = transform.GetMatrix()._33 * 0.5f;
				newPos._34 = transform.GetMatrix()._34;
				newPos._41 = transform.GetMatrix()._41;
				newPos._42 = transform.GetMatrix()._42;
				newPos._43 = transform.GetMatrix()._43;
				newPos._44 = transform.GetMatrix()._44;
				switch(i) {
					case 0:
						newPos._41 += ((transform.GetMatrix()._21 * 0.2f) + (transform.GetMatrix()._31 * 0.1f));
						newPos._42 += ((transform.GetMatrix()._22 * 0.2f) + (transform.GetMatrix()._32 * 0.1f));
						newPos._43 += ((transform.GetMatrix()._23 * 0.2f) + (transform.GetMatrix()._33 * 0.1f));
						break;
					case 1:
						newPos._41 += ((-transform.GetMatrix()._11 * 0.2f) + (transform.GetMatrix()._31 * 0.1f));
						newPos._42 += ((-transform.GetMatrix()._12 * 0.2f) + (transform.GetMatrix()._32 * 0.1f));
						newPos._43 += ((-transform.GetMatrix()._13 * 0.2f) + (transform.GetMatrix()._33 * 0.1f));
						break;
					case 2:
						newPos._41 += ((transform.GetMatrix()._11 * 0.2f) + (transform.GetMatrix()._31 * 0.1f));
						newPos._42 += ((transform.GetMatrix()._12 * 0.2f) + (transform.GetMatrix()._32 * 0.1f));
						newPos._43 += ((transform.GetMatrix()._13 * 0.2f) + (transform.GetMatrix()._33 * 0.1f));
						break;
					case 3:
						newPos._41 += ((-transform.GetMatrix()._21 * 0.2f) + (transform.GetMatrix()._31 * 0.1f));
						newPos._42 += ((-transform.GetMatrix()._22 * 0.2f) + (transform.GetMatrix()._32 * 0.1f));
						newPos._43 += ((-transform.GetMatrix()._23 * 0.2f) + (transform.GetMatrix()._33 * 0.1f));
						break;
				}

				displayItems[i]->transform.SetMatrix(newPos);
			}
		}
		*justTouched = true;
	}
	else {
		if(*justTouched) {
			for(unsigned int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
				if(displayItems[i]) displayItems[i]->Render(false);
			}
			*justTouched = false;
		}
	}
}

void ControllerObject::AddItem(int itemSlot, unsigned prefabID) {
	AddToInventory(itemSlot, prefabID);

	Gun* gun = dynamic_cast<Gun*>(items[itemSlot]);
	BuildTool* tool = dynamic_cast<BuildTool*>(items[itemSlot]);
	if(gun) {
		gun->SetStats(Gun::FireType::SEMI, 60, 1);
	}
}
void ControllerObject::AddItem(int itemSlot, unsigned prefabID, std::vector<unsigned> prefabIDs) {
	AddToInventory(itemSlot, prefabID);

	Gun* gun = dynamic_cast<Gun*>(items[itemSlot]);
	BuildTool* buildTool = dynamic_cast<BuildTool*>(items[itemSlot]);
	if(gun) {
		gun->SetStats(Gun::FireType::SEMI, 60, 1);
	}
	else if(buildTool) {
		buildTool->SetPrefabs(prefabIDs);
	}
}
void ControllerObject::AddItem(int itemSlot, unsigned prefabID, Gun::FireType _fireType, float _fireRate, float _damage) {
	AddToInventory(itemSlot, prefabID);

	Gun* gun = dynamic_cast<Gun*>(items[itemSlot]);
	if(gun) {
		gun->SetStats(_fireType, _fireRate, _damage);
	}
}

void ControllerObject::Update() {
	if (hand == HAND_Invalid) return;
	float dt = (float)GhostTime::DeltaTime();
	if (menuController) menuController->Render(false);

	//All Inputs
	Control attack = (hand == HAND_Left ? leftAttack : rightAttack);
	Control cyclePrefab = (hand == HAND_Left ? leftCyclePrefab : rightCyclePrefab);
	SwitchCurrentItem();

	//VR Inputs
	if (player->IsVR()) {
		DisplayInventory();

		static bool teleport = false;
		if (KeyIsDown(teleportDown) && hand == HAND_Right) {
			ArcCast(this, {});
			teleport = true;
		}
		if (teleport && !KeyIsDown(teleportDown) && hand == HAND_Right) {
			player->Teleport();
			teleport = false;
		}
	}
	//Keboard Inputs
	else {
		DirectX::XMFLOAT4X4 newPos = player->transform.GetMatrix();
		//Move Controllers
		float forwardDist = 1;
		{
			newPos._41 += newPos._31 * forwardDist;
			newPos._42 += newPos._32 * forwardDist;
			newPos._43 += newPos._33 * forwardDist;
		}
		float heightDist = 0.4f;
		{
			newPos._41 -= newPos._21 * heightDist;
			newPos._42 -= newPos._22 * heightDist;
			newPos._43 -= newPos._23 * heightDist;
		}
		float sideDist = 0.4f;
		{
			newPos._41 += newPos._11 * (hand == HAND_Left ? -sideDist : sideDist);
			newPos._42 += newPos._12 * (hand == HAND_Left ? -sideDist : sideDist);
			newPos._43 += newPos._13 * (hand == HAND_Left ? -sideDist : sideDist);
		}

		//Find Angle of Controllers
		float maxDist = 100;
		DirectX::XMFLOAT3 startPoint = { player->transform.GetMatrix()._41, player->transform.GetMatrix()._42, player->transform.GetMatrix()._43 };
		DirectX::XMFLOAT3 direction = { player->transform.GetMatrix()._31, player->transform.GetMatrix()._32, player->transform.GetMatrix()._33 };
		DirectX::XMFLOAT3 colPoint;
		if (!Raycast(startPoint, direction, &colPoint, nullptr, maxDist)) {
			colPoint = { startPoint.x + (direction.x * maxDist), startPoint.y + (direction.y * maxDist), startPoint.z + (direction.z * maxDist) };
		}

		//Set Position and angle of controllers
		transform.SetMatrix(newPos);
		transform.LookAt(colPoint);

		//Keyboard Inputs
		if (KeyIsDown(Control::TestInputU)) {
			DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
			newPos._42 += dt;
			transform.SetMatrix(newPos);
			//ResetKey(Control::TestInputU);
		}
		if (KeyIsDown(Control::TestInputO)) {
			DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
			newPos._42 -= dt;
			transform.SetMatrix(newPos);
			//ResetKey(Control::TestInputO);
		}
		if (KeyIsDown(Control::TestInputI)) {
			DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
			newPos._43 += dt;
			transform.SetMatrix(newPos);
			//ResetKey(Control::TestInputI);
		}
		if (KeyIsDown(Control::TestInputK)) {
			DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
			newPos._43 -= dt;
			transform.SetMatrix(newPos);
			//ResetKey(Control::TestInputK);
		}
		if (KeyIsDown(Control::TestInputJ)) {
			DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
			newPos._41 -= dt;
			transform.SetMatrix(newPos);
			//ResetKey(Control::TestInputJ);
		}
		if (KeyIsDown(Control::TestInputL)) {
			DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
			newPos._41 += dt;
			transform.SetMatrix(newPos);
			//ResetKey(Control::TestInputL);
		}
	}


	//Update Items
	#pragma region Update Inactive Items
		for (unsigned int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
			if (items[i] && items[i] != currentGameItem) {
				items[i]->InactiveUpdate();
			}
		}
	#pragma endregion
	#pragma region Update Current Item
			if (currentGameItem) {
				currentGameItem->transform.SetMatrix(transform.GetMatrix());
				currentGameItem->ActiveUpdate();

				//Specific States
				switch (currentGameItem->state) {
					case Item::State::GUN:
						{
							if (KeyIsDown(attack)) {
								if (!((Gun*)currentGameItem)->Shoot()) {
									ResetKey(attack);
								}
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

			}
	#pragma endregion

	GameObject::Update();
}
void ControllerObject::PausedUpdate() {
	if(hand == HAND_Invalid) return;
	Control attack = (hand == HAND_Left ? leftAttack : rightAttack);

	menuController->Render(true);
	menuController->transform.SetMatrix(transform.GetMatrix());

	if(KeyIsDown(attack)) {
		menuController->Activate();
		//ResetKey(attack);
	}
	else {
		menuController->UpdateRay();
	}
}

void ControllerObject::GivePID(unsigned pid, const char* tag) {

	//This is should be changed if other data is to be passed in.
	//The stupid setup of this is taking the character '1'
	itemPrefabs[*tag - '0'] = pid;
}

void ControllerObject::Awake(Object* obj) {
	memcpy(itemPrefabs, ((ControllerObject*)obj)->itemPrefabs, sizeof(unsigned) * CONTROLLER_MAX_ITEMS);
	for(int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
		if(itemPrefabs[i] > 0) {
			AddItem(i, itemPrefabs[i]);
		}
	}
}

// TEMPORARY - CHANGE OR REMOVE LATER
void ControllerObject::SetBuildItems(std::vector<unsigned> prefabIDs) {
	BuildTool* buildTool = (BuildTool*)items[3];
	assert(buildTool);
	buildTool->SetPrefabs(prefabIDs);
}

// TEMPORARY - CHANGE OR REMOVE LATER
void ControllerObject::SetGunData(int slot, Gun::FireType _fireType, float _fireRate, float _damage) {
	Gun* gun = (Gun*)items[slot];
	gun->SetStats(_fireType, _fireRate, _damage);

}


void ControllerObject::Enable(bool destroyOnEnd) {
	menuController->Render(false);
	GameObject::Enable(destroyOnEnd);
}