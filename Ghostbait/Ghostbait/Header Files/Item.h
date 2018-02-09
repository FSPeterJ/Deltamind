#pragma once
#include "GameObject.h"

class Item: public GameObject {
public:
	enum State {
		INVALID,
		GUN,
		CONTROLLER,
		BUILD,
		HAND,
	};
	State state;
	Item();

	virtual void InactiveUpdate() {};
	virtual void ActiveUpdate() {};
};
