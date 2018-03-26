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
#include "HexGrid.h"
#include "Material.h"
//Only here to safely instantiate something. Should be done differently later
#include "ObjectFactory.h"

ControllerObject::ControllerObject() {
}

void ControllerObject::Init(Player* _player, ControllerHand _hand) {
	player = _player;
	hand = _hand;

	//Create MenuController
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuControllerItem>({ 0,0,0 }, &menuController));
	menuController->UnRender();
	//Create ModelOnly controller
	unsigned modelOnlyID;
	if(player->IsVR()) modelOnlyID = ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost"));
	else modelOnlyID = ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost"));
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Item>(modelOnlyID, { 0,0,0 }, &modelOnly));
	modelOnly->UnRender();

	//Assign current item
	for (int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
		if (inventory.items[i]) {
			inventory.currentItem = inventory.items[0];
			break;
		}
	}

	//Tell all controller items not to destroy on reset
	menuController->PersistOnReset();
	modelOnly->PersistOnReset();
	for (int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
		if (inventory.items[i]) {
			inventory.items[i]->PersistOnReset();
		}
	}
	PersistOnReset();
}
void ControllerObject::Awake(Object* obj) {
	memcpy(inventory.itemPrefabs, ((ControllerObject*)obj)->inventory.itemPrefabs, sizeof(unsigned) * CONTROLLER_MAX_ITEMS);
	for(int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
		if(inventory.itemPrefabs[i] > 0) {
			AddItem(i, inventory.itemPrefabs[i]);
		}
	}
}
void ControllerObject::GivePID(unsigned pid, const char* tag) {

	//This is should be changed if other data is to be passed in.
	//The stupid setup of this is taking the character '1'
	inventory.itemPrefabs[*tag - '0'] = pid;
}


void ControllerObject::SwitchCurrentItem(int itemIndex) {
	if(itemIndex == -1) {
		Control item0 = (hand == HAND_Left ? leftItem0 : rightItem0);
		Control item1 = (hand == HAND_Left ? leftItem1 : rightItem1);
		Control item2 = (hand == HAND_Left ? leftItem2 : rightItem2);
		Control item3 = (hand == HAND_Left ? leftItem3 : rightItem3);
		if(Amount(item0) == 1) {
			if(inventory.items[0]) {
				SwitchCurrentItem(0);
				ResetKey(item0);
				return;
			}
		}
		if(Amount(item1) == 1) {
			if(inventory.items[1]) {
				SwitchCurrentItem(1);
				ResetKey(item1);
				return;
			}
		}
		if(Amount(item2) == 1) {
			if(inventory.items[2]) {
				SwitchCurrentItem(2);
				ResetKey(item2);
				return;
			}
		}
		if(Amount(item3) == 1) {
			if(inventory.items[3]) {
				SwitchCurrentItem(3);
				ResetKey(item3);
				return;
			}
		}
	}
	else if(inventory.items[itemIndex] != inventory.currentItem) {
		inventory.currentItem->DeSelected();
		inventory.currentItem = inventory.items[itemIndex];
		inventory.currentItem->Selected();
		return;
	}
}
void ControllerObject::DisplayInventory() {
	static bool leftJustTouched = false, rightJustTouched = false;
	inventory.displayRotation += (float)GhostTime::DeltaTime() * 0.5f;

	bool* justTouched = (hand == HAND_Left ? &leftJustTouched : &rightJustTouched);
	Control touch = (hand == HAND_Left ? leftTouch : rightTouch);
	Control item0 = (hand == HAND_Left ? leftItem0 : rightItem0);
	Control item1 = (hand == HAND_Left ? leftItem1 : rightItem1);
	Control item2 = (hand == HAND_Left ? leftItem2 : rightItem2);
	Control item3 = (hand == HAND_Left ? leftItem3 : rightItem3);

	if(KeyIsDown(touch)) {
		for(unsigned int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
			if(inventory.displayItems[i]) {
				if(!*justTouched) inventory.displayItems[i]->Render();
				DirectX::XMFLOAT4X4 newPos;
				DirectX::XMMATRIX result, scale, rotation, translation, parentMatrix;
				result = DirectX::XMMatrixIdentity();
				scale = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
				rotation = DirectX::XMMatrixIdentity();
				parentMatrix = DirectX::XMLoadFloat4x4(&transform.GetMatrix());

				switch(i) {
					case 0:
						translation = DirectX::XMMatrixTranslation(0, 0.2f, 0.1f);
						if (Amount(item0) == 0.5f) {
							if (inventory.currentSpinningItem != 0) {
								inventory.displayRotation = 0;
								inventory.currentSpinningItem = 0;
							}
							rotation = DirectX::XMMatrixRotationRollPitchYaw(0, inventory.displayRotation * 2, 0);
						}
						break;
					case 1:
						translation = DirectX::XMMatrixTranslation(-0.2f, 0, 0.1f);
						if (Amount(item1) == 0.5f) {
							if (inventory.currentSpinningItem != 1) {
								inventory.displayRotation = 0;
								inventory.currentSpinningItem = 1;
							}
							rotation = DirectX::XMMatrixRotationRollPitchYaw(0, inventory.displayRotation * 2, 0);
						}
						break;
					case 2:
						translation = DirectX::XMMatrixTranslation(0.2f, 0, 0.1f);
						if (Amount(item2) == 0.5f) {
							if (inventory.currentSpinningItem != 2) {
								inventory.displayRotation = 0;
								inventory.currentSpinningItem = 2;
							}
							rotation = DirectX::XMMatrixRotationRollPitchYaw(0, inventory.displayRotation * 2, 0);
						}
						break;
					case 3:
						translation = DirectX::XMMatrixTranslation(0, -0.2f, 0.1f);
						if (Amount(item3) == 0.5f) {
							if (inventory.currentSpinningItem != 3) {
								inventory.displayRotation = 0;
								inventory.currentSpinningItem = 3;
							}
							rotation = DirectX::XMMatrixRotationRollPitchYaw(0, inventory.displayRotation * 2, 0);
						}
						break;
				}

				result = scale * rotation * translation * parentMatrix;
				DirectX::XMStoreFloat4x4(&newPos, result);
				inventory.displayItems[i]->transform.SetMatrix(newPos);
			}
		}
		*justTouched = true;
	}
	else {
		if(*justTouched) {
			for(unsigned int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
				if(inventory.displayItems[i]) inventory.displayItems[i]->UnRender();
			}
			*justTouched = false;
		}
	}
}


void ControllerObject::AddToInventory(int itemSlot, unsigned prefabID) {
	if (!inventory.items[itemSlot]) ++inventory.itemCount;
	//Actual Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Item>(prefabID, { 0,0,0 }, (Item**)&inventory.items[itemSlot]));
	if(!inventory.currentItem) {
		inventory.currentItem = inventory.items[itemSlot];
		inventory.currentItem->Selected();
	}
	inventory.items[itemSlot]->UnRender();
	inventory.items[itemSlot]->PersistOnReset();
	inventory.items[itemSlot]->SetPhysicsComponent(false);

	//Inventory Display
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Item>(prefabID, { 0,0,0 }, (Item**)&inventory.displayItems[itemSlot]));
	inventory.displayItems[itemSlot]->UnRender();
	inventory.displayItems[itemSlot]->PersistOnReset();
	inventory.displayItems[itemSlot]->SetPhysicsComponent(false);


}

void ControllerObject::RemoveItem(int itemSlot) {
	if (inventory.items[itemSlot]) {
		inventory.items[itemSlot] = nullptr;
		inventory.displayItems[itemSlot] = nullptr;
		--inventory.itemCount;
	}
}
void ControllerObject::AddItem(int itemSlot, unsigned prefabID) {
	AddToInventory(itemSlot, prefabID);

	Gun* gun = dynamic_cast<Gun*>(inventory.items[itemSlot]);
	BuildTool* tool = dynamic_cast<BuildTool*>(inventory.items[itemSlot]);
	if(gun) {
		gun->SetStats(Gun::FireType::SEMI, 60, 1);
		gun->overheat.CreateBar(gun);
	}
}
void ControllerObject::AddItem(int itemSlot, unsigned prefabID, std::vector<unsigned> prefabIDs) {
	AddToInventory(itemSlot, prefabID);

	Gun* gun = dynamic_cast<Gun*>(inventory.items[itemSlot]);
	BuildTool* buildTool = dynamic_cast<BuildTool*>(inventory.items[itemSlot]);
	if(gun) {
		gun->SetStats(Gun::FireType::SEMI, 60, 1);
		gun->overheat.CreateBar(gun);
	}
	else if(buildTool) {
		buildTool->SetPrefabs(prefabIDs);
	}
}
void ControllerObject::AddItem(int itemSlot, unsigned prefabID, Gun::FireType _fireType, float _fireRate, float _damage) {
	AddToInventory(itemSlot, prefabID);

	Gun* gun = dynamic_cast<Gun*>(inventory.items[itemSlot]);
	if(gun) {
		gun->SetStats(_fireType, _fireRate, _damage);
		gun->overheat.CreateBar(gun);
	}
}

void ControllerObject::SetControllerState(ControllerState newState) {
	prevState = state;
	state = newState;

	//Deactivate old state data
	switch (prevState) {
		case ControllerState::CSTATE_Inventory:
			{
				int startItemIndex = 0;
				for (int i = CONTROLLER_MAX_ITEMS - 1; i >= 0 ; --i) {
					if (inventory.displayItems[i]) {
						startItemIndex = i;
						inventory.displayItems[i]->UnRender();
					}
				}
				inventory.currentItem->DeSelected();
				//inventory.currentItem = inventory.items[startItemIndex];
			}
			break;
		case ControllerState::CSTATE_MenuController:
			{
				menuController->DeSelected();
			}
			break;
		case ControllerState::CSTATE_ModelOnly:
			{
				modelOnly->DeSelected();
			}
			break;
		case ControllerState::CSTATE_None:
			{

			}
			break;
	}
	//Activate new state data
	switch (newState) {
		case ControllerState::CSTATE_Inventory:
			{
				inventory.currentItem->Selected();
			}
			break;
		case ControllerState::CSTATE_MenuController:
			{
				menuController->Selected();
			}
			break;
		case ControllerState::CSTATE_ModelOnly:
			{
				modelOnly->Selected();
			}
			break;
		case ControllerState::CSTATE_None:
			{

			}
			break;
	}
}
void ControllerObject::SetControllerStateToPrevious() {
	SetControllerState(prevState);
}

void ControllerObject::Update() {
	if (hand == HAND_Invalid) return;
	float dt = (float)GhostTime::DeltaTime();

	//All Inputs

	switch (state) {
		case ControllerState::CSTATE_Inventory:
			{
				Control attack = (hand == HAND_Left ? leftAttack : rightAttack);
				Control cyclePrefab = (hand == HAND_Left ? leftCyclePrefab : rightCyclePrefab);

				if(!player->IsGod())
					SwitchCurrentItem();

				//VR Inputs
				if (player->IsVR()) {
					DisplayInventory();

					if (!player->IsGod()) {
						static bool teleportQueued = false;
						DirectX::XMFLOAT3 endPos;
						if (KeyIsDown(teleportDown) && hand == HAND_Right) {
							if (ArcCast(&transform, &endPos, &player->teleportArc)) {
								player->teleportArc.Create();
								if (!(player->GetBuildGrid()->IsBlocked(DirectX::XMFLOAT2(endPos.x, endPos.z))) && (player->teleportArc.Get()->componentVarients.find("valid") != player->teleportArc.Get()->componentVarients.end())) {
									int id = TypeMap::GetComponentTypeID<Material>();
									player->teleportArc.Get()->SetComponent(player->teleportArc.Get()->componentVarients["valid"], id);
									teleportQueued = true;
								}
								else if (player->teleportArc.Get()->componentVarients.find("invalid") != player->teleportArc.Get()->componentVarients.end()) {
									int id = TypeMap::GetComponentTypeID<Material>();
									player->teleportArc.Get()->SetComponent(player->teleportArc.Get()->componentVarients["invalid"], id);
									teleportQueued = false;
								}
							}
							else {
								player->teleportArc.Destroy();
								teleportQueued = false;
							}
						}
						else if (!KeyIsDown(teleportDown) && hand == HAND_Right) {
							player->teleportArc.Destroy();
						}
						if (teleportQueued && !KeyIsDown(teleportDown) && hand == HAND_Right) {
							player->teleportArc.Destroy();
							player->Teleport();
							teleportQueued = false;
						}
					}
				}
				//Keboard Inputs
				else {
					PositionNonVRController();

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
						if (inventory.items[i] && inventory.items[i] != inventory.currentItem) {
							inventory.items[i]->InactiveUpdate();
						}
					}
				#pragma endregion
				#pragma region Update Current Item
				if (inventory.currentItem) {
					inventory.currentItem->transform.SetMatrix(transform.GetMatrix());
					inventory.currentItem->ActiveUpdate();

					//Specific States
					switch (inventory.currentItem->state) {
						case Item::State::GUN:
							{
								if (KeyIsDown(attack)) {
									if (!((Gun*)inventory.currentItem)->Shoot()) {
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
									((BuildTool*)inventory.currentItem)->CycleForward();
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
									((BuildTool*)inventory.currentItem)->Activate();
									//if (hand == HAND_Left) leftAttacked = true;
									//else if (hand == HAND_Right) rightAttacked = true;
									ResetKey(attack);
								}
								if (!KeyIsDown(attack)) {
									((BuildTool*)inventory.currentItem)->Projection();
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
			}
			break;
		case ControllerState::CSTATE_MenuController:
			{
				Control attack = (hand == HAND_Left ? leftAttack : rightAttack);

				if (!player->IsVR()) PositionNonVRController();
				menuController->transform.SetMatrix(transform.GetMatrix());
				
				if (KeyIsDown(attack)) {
					ResetKey(attack);
						menuController->Activate();
					}
				else {
						menuController->UpdateRay();
					}
			}
			break;
		case ControllerState::CSTATE_ModelOnly:
			{
				if (!player->IsVR()) PositionNonVRController();
				modelOnly->transform.SetMatrix(transform.GetMatrix());
			}
			break;
		case ControllerState::CSTATE_None:
			break;
	}

	GameObject::Update();
}


// TEMPORARY - CHANGE OR REMOVE LATER
void ControllerObject::SetBuildItems(std::vector<unsigned> prefabIDs) {
	BuildTool* buildTool = (BuildTool*)inventory.items[3];
	assert(buildTool);
	buildTool->SetPrefabs(prefabIDs);
}

// TEMPORARY - CHANGE OR REMOVE LATER
void ControllerObject::SetGunData(int slot, Gun::FireType _fireType, float _fireRate, float _damage) {
	Gun* gun = (Gun*)inventory.items[slot];
	gun->SetStats(_fireType, _fireRate, _damage);

}


void ControllerObject::Enable() {
	GameObject::Enable();
}

void ControllerObject::PositionNonVRController() {
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
	DirectX::XMFLOAT3 direction = { player->transform.GetMatrix()._31, player->transform.GetMatrix()._32, player->transform.GetMatrix()._33 };
	DirectX::XMFLOAT3 colPoint;
	if (!Raycast(&player->transform, direction, &colPoint, nullptr, nullptr, maxDist)) {
		colPoint = { player->transform.GetPosition().x + (direction.x * maxDist), player->transform.GetPosition().y + (direction.y * maxDist), player->transform.GetPosition().z + (direction.z * maxDist) };
	}

	//Set Position and angle of controllers
	transform.SetMatrix(newPos);
	transform.LookAt(colPoint);
}

BuildTool* ControllerObject::GetBuildTool() { 
	BuildTool* build = nullptr;
	for (int i = 0; i < CONTROLLER_MAX_ITEMS; ++i) {
		build = dynamic_cast<BuildTool*>(inventory.items[i]);
		if (build) return build;
	}
	return nullptr;
}

