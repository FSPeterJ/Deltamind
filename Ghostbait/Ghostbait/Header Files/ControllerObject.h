#pragma once
#include "GameObject.h"
#include "Gun.h"
#include "Controlable.h"  // for Controlable
#include "vector"         // for vector
#include "MenuControllerItem.h"

#define CONTROLLER_MAX_ITEMS 8

class ControllerObject: public GameObject, public Controlable {
public:
	enum ControllerHand {
		HAND_Invalid,
		HAND_Left,
		HAND_Right,
	};
private:
	MenuControllerItem* menuController = nullptr;
	ControllerHand hand = HAND_Invalid;
	Item* items[CONTROLLER_MAX_ITEMS] = { 0 };
	Item* displayItems[CONTROLLER_MAX_ITEMS] = { 0 };
	Item* currentGameItem = nullptr;

	//Find a better solution for this
	unsigned itemPrefabs[CONTROLLER_MAX_ITEMS] = { 0 };

	void SetPhysicsComponent(const GameObject* obj, bool active);
	void AddToInventory(int itemSlot, unsigned prefabID);
	inline void SetControllerHand(ControllerHand _hand) { hand = _hand; };

	void SwitchCurrentItem(int itemIndex = -1);
	void DisplayInventory();
public:

	ControllerObject();

	void Init(ControllerHand _hand);
	void AddItem(int itemSlot, unsigned prefabID);
	void AddItem(int itemSlot, unsigned prefabID, std::vector<unsigned> prefabIDs);
	void AddItem(int itemSlot, unsigned prefabID, Gun::FireType _fireType, float _fireRate, float _damage);
	void Update();
	void PausedUpdate();
	void GivePID(unsigned pid, char* tag);
	void CloneData(Object* obj);

	//TEMPORARY FUNCTION
	void SetBuildItems(std::vector<unsigned> prefabIDs);
	void SetGunData(int slot, Gun::FireType _fireType, float _fireRate, float _damage);
};
