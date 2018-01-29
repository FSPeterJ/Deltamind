#pragma once
#include "GameObject.h"

class Item : public GameObject {
public:
	enum State {
		INVALID,
		GUN,
		CONTROLLER,
		HAND,
	};
	State state;
	Item();
};