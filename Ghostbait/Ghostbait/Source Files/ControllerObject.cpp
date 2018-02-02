#include "ControllerObject.h"
#include "Console.h"         // for Console, Console::WriteLine, Console::WriteLiner
#include "MessageEvents.h"

ControllerObject::ControllerObject() {
	items.resize(4);
	displayItems.resize(4);
	hand = INVALID;
}
void ControllerObject::AddGun(int itemSlot, int prefabID, Gun::FireType _fireType, float _fireRate, float _damage) {
	//Add Item to Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**) &items[itemSlot]));
	((Gun*) items[itemSlot])->SetStats(_fireType, _fireRate, _damage);
	if(!currentItem) currentItem = items[itemSlot];
	else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));

	//Add Item to display Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&displayItems[itemSlot]));
	MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(displayItems[itemSlot]));
};
void ControllerObject::AddController(int itemSlot, int prefabID) {
	//Add Item to Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, {0,0,0}, (GameObject**) &items[itemSlot]));
	if(!currentItem) currentItem = items[itemSlot];
	else MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(items[itemSlot]));

	//Add Item to display Inventory
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0,0,0 }, (GameObject**)&displayItems[itemSlot]));
	MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(displayItems[itemSlot]));

};
void ControllerObject::Update() {
	if(hand == INVALID) return;
	else if(hand == LEFT) LeftUpdate();
	else RightUpdate();
}
void ControllerObject::LeftUpdate() {
	static bool touchHeld = false;
	#pragma region Switch Controller Item
	if(KeyIsDown(leftItem1)) {
		if(items[0]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[0];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(leftItem1);
	} else if(KeyIsDown(leftItem2)) {
		if(items[1]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[1];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(leftItem2);
	} else if(KeyIsDown(leftItem3)) {
		if(items[2]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[2];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(leftItem3);
	} else if(KeyIsDown(leftItem4)) {
		if(items[3]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[3];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(leftItem4);
	}
#pragma endregion
	#pragma region Display Inventory
		if (KeyIsDown(leftTouch)) {
		for (int i = 0; i < displayItems.size(); ++i) {
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
		for (int i = 0; i < displayItems.size(); ++i) {
			if (displayItems[i]) 
				MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(displayItems[i]));
		}
		touchHeld = false;
	}
	#pragma endregion

	if(currentItem) {
		currentItem->position = position;
		currentItem->Update();
		switch(currentItem->state) {
		case Item::State::GUN:
			if(KeyIsDown(leftAttack)) {
				if(!((Gun*) currentItem)->Shoot()) ResetKey(leftAttack);
			}
			break;
		case Item::State::CONTROLLER:
			if(KeyIsDown(leftAttack)) {
				//Console::WriteLine << "Right controller select";
				ResetKey(leftAttack);
			}
			break;
		case Item::State::INVALID:
			if(KeyIsDown(leftAttack)) {
				//Console::WriteLine << "Right hand pickup";
				ResetKey(leftAttack);
			}
			break;
		}
	}
}
void ControllerObject::RightUpdate() {
	static bool touchHeld = false;
	#pragma region Switch Controller Item
	if(KeyIsDown(rightItem1)) {
		if(items[0]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[0];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(rightItem1);
	} else if(KeyIsDown(rightItem2)) {
		if(items[1]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[1];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(rightItem2);
	} else if(KeyIsDown(rightItem3)) {
		if(items[2]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[2];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(rightItem3);
	} else if(KeyIsDown(rightItem4)) {
		if(items[3]) {
			MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(currentItem));
			currentItem = items[3];
			MessageEvents::SendMessage(EVENT_Addrender, DestroyMessage(currentItem));
		}
		ResetKey(rightItem4);
	}
#pragma endregion
	#pragma region Display Inventory
		if (KeyIsDown(rightTouch)) {
			for (int i = 0; i < displayItems.size(); ++i) {
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
			for (int i = 0; i < displayItems.size(); ++i) {
				if (displayItems[i])
					MessageEvents::SendMessage(EVENT_Unrender, DestroyMessage(displayItems[i]));
			}
			touchHeld = false;
		}
	#pragma endregion
	if(currentItem) {
		currentItem->position = position;
		currentItem->Update();
		switch(currentItem->state) {
		case Item::State::GUN:
			if(KeyIsDown(rightAttack)) {
				if(!((Gun*) currentItem)->Shoot()) ResetKey(rightAttack);
			}
			break;
		case Item::State::CONTROLLER:
			if(KeyIsDown(rightAttack)) {
				//Console::WriteLine << "Right controller select";
				ResetKey(leftAttack);
			}
			break;
		case Item::State::INVALID:
			if(KeyIsDown(rightAttack)) {
				//Console::WriteLine << "Right hand pickup";
				ResetKey(rightAttack);
			}
			break;
		}
	}
}
