#pragma once
#include "GameObject.h"

class Item: public GameObject {
	bool isRendered = true;
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

	void Render(bool render = true);
	virtual void InactiveUpdate() {};
	virtual void ActiveUpdate() {};
};
