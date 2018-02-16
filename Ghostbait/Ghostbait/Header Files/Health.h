#pragma once
#include "GameObject.h"

class Health {
	float maxHealth;
	float health;
	void Clamp(float min, float max, float* val);
public:
	Health();
	Health(float _maxHealth);
	Health(float _maxHealth, float _startingHealth);

	//Purpose: 
	//	Get the current health as a percentage
	//Returns:
	//	the currentHealth / the maxHealth
	float PercentHealth();
	//Purpose: 
	//	Add or remove health from object
	//Parameters:
	//	float amount - the amount of health to add(positive) remove(negative)
	void AdjustHealth(float amount);
	//Purpose: 
	//	See if the object still has health
	//Returns:
	//	true if the object still has health, false if the object has <= 0 health
	bool IsAlive();
};