#pragma once
#include "GameObject.h"
#include "Gun.h"
#include "Controlable.h"  // for Controlable
#include "vector"         // for vector
#include "MenuControllerItem.h"

class ControllerObject: public GameObject, public Controlable {
public:
	enum ControllerHand {
		HAND_Invalid,
		HAND_Left,
		HAND_Right,
	};
private:
	MenuControllerItem* menuController;
	ControllerHand hand = HAND_Invalid;
	std::vector<Item*> items;
	std::vector<Item*> displayItems;
	Item* currentGameItem = nullptr;

	void SetPhysicsComponent(const GameObject* obj, bool active);
	void AddToInventory(int itemSlot, char* prefabName);
	inline void SetControllerHand(ControllerHand _hand) { hand = _hand; };

	void SwitchCurrentItem(int itemIndex = -1);
	void DisplayInventory();
public:

	ControllerObject();

	void Init(ControllerHand _hand, int menuControllerPrefabID = 18);
	void AddItem(int itemSlot, char* prefabName);
	void AddItem(int itemSlot, char* prefabName, std::vector<int> prefabIDs);
	void AddItem(int itemSlot, char* prefabName, Gun::FireType _fireType, float _fireRate, float _damage);
	void Update();
	void PausedUpdate();
};
