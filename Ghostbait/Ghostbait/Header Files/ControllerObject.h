#pragma once
#include "GameObject.h"
#include "Gun.h"
#include "Controlable.h"  // for Controlable
#include "vector"         // for vector

class ControllerObject : public GameObject, public Controlable {
public:
	enum ControllerHand {
		INVALID,
		LEFT,
		RIGHT,
	};
private:

	ControllerHand hand = LEFT;
	std::vector<Item*> items;
	Item* currentItem = nullptr;
	void LeftUpdate();
	void RightUpdate();
public:

	ControllerObject();
	void AddGun(int itemSlot, int prefabID, Gun::FireType _fireType, float _fireRate, float _damage);
	void AddController(int itemSlot, int prefabID);
	inline void SetControllerHand(ControllerHand _hand) { hand = _hand; };
	void Update();
};
