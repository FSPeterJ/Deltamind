#pragma once
#include "GameObject.h"
#include "Console.h"

class Ground : public GameObject {
public:
	Ground() {
		SetTag("Ground");
	}
};