#pragma once
#include "GameObject.h"
#include "Gun.h"
#include "Controlable.h"  // for Controlable
#include "vector"         // for vector
#include "MenuControllerItem.h"

class ControllerObject: public GameObject, public Controlable {
public:
	enum ControllerHand {
		INVALID,
		LEFT,
		RIGHT,
	};
private:
	bool menuControllerVisible = false;
	MenuControllerItem* menuController;
	ControllerHand hand = LEFT;
	std::vector<Item*> items;
	std::vector<Item*> displayItems;
	Item* currentGameItem = nullptr;
	bool touchHeld = false;

	void SetPhysicsComponent(GameObject* obj, bool active);
	void AddToInventory(int itemSlot, int prefabID);
public:

	ControllerObject();

	void Init(ControllerHand _hand, int menuControllerPrefabID = 1);
	void AddItem(int itemSlot, int prefabID);
	void AddItem(int itemSlot, int prefabID, std::vector<int> prefabIDs);
	void AddItem(int itemSlot, int prefabID, Gun::FireType _fireType, float _fireRate, float _damage);
	inline void SetControllerHand(ControllerHand _hand) { hand = _hand; };
	void Update();
	void PausedUpdate();
};
