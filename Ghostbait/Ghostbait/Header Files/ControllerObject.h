#pragma once
#include "GameObject.h"
#include "Gun.h"
#include "Controlable.h"  // for Controlable
#include "vector"         // for vector

class ControllerObject: public GameObject, public Controlable {
public:
	enum ControllerHand {
		INVALID,
		LEFT,
		RIGHT,
	};
private:
	ControllerHand hand = LEFT;
	std::vector<Item*> items;
	std::vector<Item*> displayItems;
	Item* currentGameItem = nullptr;
	bool touchHeld = false;
public:

	ControllerObject();
	void AddItem(int itemSlot, int prefabID);
	void AddItem(int itemSlot, int prefabID, std::vector<unsigned> prefabIDs);
	void AddItem(int itemSlot, int prefabID, Gun::FireType _fireType, float _fireRate, float _damage);
	inline void SetControllerHand(ControllerHand _hand) { hand = _hand; };
	void Update();
};
