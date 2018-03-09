#pragma once
#include "GameObject.h"
#include "Gun.h"
#include "Controlable.h"  // for Controlable
#include "vector"         // for vector
#include "MenuControllerItem.h"
#include "VRStructs.h"

class Player;

#define CONTROLLER_MAX_ITEMS 4

enum ControllerState {
	CSTATE_None,
	CSTATE_Inventory,
	CSTATE_MenuController,
	CSTATE_ModelOnly,
};

class ControllerObject: public GameObject, public Controlable {
	struct Inventory {
		unsigned itemCount = 0;
		//Find a better solution for this
		unsigned itemPrefabs[CONTROLLER_MAX_ITEMS] = { 0 };
		Item* items[CONTROLLER_MAX_ITEMS] = { 0 };
		Item* displayItems[CONTROLLER_MAX_ITEMS] = { 0 };
		Item* currentItem = nullptr;
		float displayRotation = 0;
		int currentSpinningItem = -1;
	};

	Inventory inventory;
	MenuControllerItem* menuController = nullptr;

	Player* player = nullptr;
	ControllerState prevState = CSTATE_None;
	ControllerState state = CSTATE_None;
	ControllerHand hand = HAND_Invalid;

	void SetPhysicsComponent(const GameObject* obj, bool active);
	void AddToInventory(int itemSlot, unsigned prefabID);
	inline void SetControllerHand(ControllerHand _hand) { hand = _hand; };

	void SwitchCurrentItem(int itemIndex = -1);
	void DisplayInventory();

	void PositionNonVRController();

public:

	ControllerObject();

	void Init(Player* _player, ControllerHand _hand);
	void AddItem(int itemSlot, unsigned prefabID);
	void AddItem(int itemSlot, unsigned prefabID, std::vector<unsigned> prefabIDs);
	void AddItem(int itemSlot, unsigned prefabID, Gun::FireType _fireType, float _fireRate, float _damage);
	void Update();
	void PausedUpdate();
	void GivePID(unsigned pid, const char* tag) override;
	void Awake(Object* obj);

	//TEMPORARY FUNCTION
	void SetBuildItems(std::vector<unsigned> prefabIDs);
	void SetGunData(int slot, Gun::FireType _fireType, float _fireRate, float _damage);
	void Enable(bool destroyOnEnd);

	void SetControllerState(ControllerState newState);
	void SetControllerStateToPrevious();
	inline const ControllerState& GetControllerState() const {return state;};
};
