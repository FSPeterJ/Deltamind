#pragma once
#include "GameObject.h"

class Ground : public GameObject {
public:
	Ground() {
		SetTag("Ground");
	}
};