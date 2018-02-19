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
	void AddToInventory(int itemSlot, unsigned prefabID);
public:

	ControllerObject();

	void Init(ControllerHand _hand, unsigned menuControllerPrefabID = 18);
	void AddItem(int itemSlot, unsigned prefabID);
	void AddItem(int itemSlot, unsigned prefabID, std::vector<unsigned> prefabIDs);
	inline void SetControllerHand(ControllerHand _hand) { hand = _hand; };
	void AddItem(int itemSlot, unsigned prefabID, Gun::FireType _fireType, float _fireRate, float _damage);
	void Update();
	void PausedUpdate();
};
