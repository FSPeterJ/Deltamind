#pragma once
#include "GameObject.h"
#include "Health.h"

class Core : public GameObject,  public Health {
	bool enemyTouched = false;
public:
	Core() { SetTag("Core"); };
	void Destroy() override;
	void Death() override;
};