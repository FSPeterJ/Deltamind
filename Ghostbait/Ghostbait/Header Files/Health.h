#pragma once
#include <mutex>

class Health {
	std::mutex healthMutex;
	float maxHealth;
	float health;
	void Clamp(float min, float max, float* val);
public:
	Health();
	Health(float _maxHealth);
	Health(float _maxHealth, float _startingHealth);

	//Purpose: 
	//	Set Max Health to value
	//Parameters:
	//	float amount - the amount to set maxHealth to
	inline void SetMaxHealth(float value) { maxHealth = value; }
	//Purpose: 
	//	Reset Health to maximum
	inline void SetToFullHealth() { health = maxHealth; }
	//Purpose: 
	//	Get the current health as a percentage
	//Returns:
	//	the currentHealth / the maxHealth
	const float PercentHealth() const;
	//Purpose: 
	//	Add or remove health from object
	//Parameters:
	//	float amount - the amount of health to add(positive) remove(negative)
	//Returns:
	//	actual amount of health adjusted
	float AdjustHealth(float amount);
	//Purpose: 
	//	See if the object still has health
	//Returns:
	//	true if the object still has health, false if the object has <= 0 health
	const bool IsAlive() const;

	//Purpose: 
	//	Child classes should override this to react to taking damage
	virtual void HurtEvent() {};
	//Purpose: 
	//	Child classes should override this to react to receiving health
	virtual void HealedEvent() {};
	//Purpose: 
	//	Child classes should override this to react to their death (when they reach 0 health)
	virtual void DeathEvent() {};
};
