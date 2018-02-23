#pragma once
#include "GameObject.h"

class Turret: public GameObject {
	GameObject* target;
	unsigned projectiePID =0;
	float targetDistance =0;
	float firerate =0;
	float damage = 10;
	float timeSinceLastShot = 0;

public:
	Turret();
	void Update();
	float CalculateDistance(GameObject* obj);
	~Turret();
	void OnTrigger(GameObject* object);
	bool CanShoot(float fireRate);
	void Shoot();
	void CloneData(Object* obj);
	void GivePID(unsigned pid, const char* tag);
};
