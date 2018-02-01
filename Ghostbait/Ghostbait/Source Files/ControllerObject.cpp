#include "ControllerObject.h"
#include <DirectXMath.h>     // for XMFLOAT4X4
#include "Console.h"         // for Console, Console::WriteLine, Console::WriteLiner
#include "Item.h"            // for Item, Item::State, Item::State::CONTROLLER, Item::State::GUN, Item::State::INVALID
#include "MessageStructs.h"  // for Control::leftAttack, Control::rightAttack, Control::leftItem1, Control::leftItem2, Control::leftItem3, Control::leftItem4, Control::rightItem1, Control::rightItem2, Control::rightItem3, Control::rightItem4
#include "MessageEvents.h"

ControllerObject::ControllerObject() {
	items.resize(4);
	hand = INVALID;
}
void ControllerObject::AddGun(int itemSlot, int prefabID, Gun::FireType _fireType, float _fireRate, float _damage) {
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&items[itemSlot]));
	((Gun*)items[itemSlot])->SetStats(_fireType, _fireRate, _damage);
	if (!currentItem) currentItem = items[itemSlot];
	else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));
};
void ControllerObject::AddController(int itemSlot, int prefabID) {
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&items[itemSlot]));
	if (!currentItem) currentItem = items[itemSlot];
	else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));
};
void ControllerObject::Update() {
	if (hand == INVALID) return;
	else if (hand == LEFT) LeftUpdate();
	else RightUpdate();
}
void ControllerObject::LeftUpdate() {
#pragma region Switch Controller Item
	if (KeyIsDown(leftItem1)) {
		if (items[0]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[0];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(leftItem1);
	}
	else if (KeyIsDown(leftItem2)) {
		if (items[1]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[1];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(leftItem2);
	}
	else if (KeyIsDown(leftItem3)) {
		if (items[2]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[2];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(leftItem3);
	}
	else if (KeyIsDown(leftItem4)) {
		if (items[3]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[3];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(leftItem4);
	}
#pragma endregion
	if (currentItem) {
		currentItem->position = position;
		currentItem->Update();
		switch (currentItem->state) {
		case Item::State::GUN:
			if (KeyIsDown(leftAttack)) {
				if (!((Gun*)currentItem)->Shoot()) ResetKey(leftAttack);
			}
			break;
		case Item::State::CONTROLLER:
			if (KeyIsDown(leftAttack)) {
				Console::WriteLine << "Right controller select";
				ResetKey(leftAttack);
			}
			break;
		case Item::State::INVALID:
			if (KeyIsDown(leftAttack)) {
				Console::WriteLine << "Right hand pickup";
				ResetKey(leftAttack);
			}
			break;
		}
	}
}
void ControllerObject::RightUpdate() {
#pragma region Switch Controller Item
	if (KeyIsDown(rightItem1)) {
		if (items[0]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[0];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(rightItem1);
	}
	else if (KeyIsDown(rightItem2)) {
		if (items[1]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[1];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(rightItem2);
	}
	else if (KeyIsDown(rightItem3)) {
		if (items[2]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[2];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(rightItem3);
	}
	else if (KeyIsDown(rightItem4)) {
		if (items[3]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[3];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(rightItem4);
	}
#pragma endregion
	if (currentItem) {
		currentItem->position = position;
		currentItem->Update();
		switch (currentItem->state) {
		case Item::State::GUN:
			if (KeyIsDown(rightAttack)) {
				if (!((Gun*)currentItem)->Shoot()) ResetKey(rightAttack);
			}
			break;
		case Item::State::CONTROLLER:
			if (KeyIsDown(rightAttack)) {
				Console::WriteLine << "Right controller select";
				ResetKey(leftAttack);
			}
			break;
		case Item::State::INVALID:
			if (KeyIsDown(rightAttack)) {
				Console::WriteLine << "Right hand pickup";
				ResetKey(rightAttack);
			}
			break;
		}
	}
}
