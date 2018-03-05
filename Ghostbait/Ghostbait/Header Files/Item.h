#pragma once
#include "GameObject.h"

class Item: public GameObject {
protected:
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

	virtual void Render(bool render = true);
	virtual void InactiveUpdate() {}
	virtual void ActiveUpdate() {}

	virtual void Selected() {}
	virtual void DeSelected() {}
};
