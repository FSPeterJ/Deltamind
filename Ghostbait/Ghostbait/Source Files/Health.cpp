#include "Health.h"

#define default_health 100

Health::Health() {
	maxHealth = default_health;
	health = default_health;
}
Health::Health(const float _maxHealth) {
	maxHealth = _maxHealth;
	health = _maxHealth;
}
Health::Health(const float _maxHealth, const float _startingHealth) {
	maxHealth = _maxHealth;
	health = _startingHealth;
	Clamp(0, maxHealth, &health);
}

void Health::Clamp(const float min, const float max, float* val) {
	if (*val < min) *val = min;
	else if (*val > max) *val = max;
}

const float Health::PercentHealth() const {
	return (health / maxHealth);
}
float Health::AdjustHealth(const float amount) {
	healthMutex.lock();
	if (amount < 0) {
		if (health == 0) {
			healthMutex.unlock();
			return 0;
		}
		HurtEvent();
	}
	if (amount > 0) HealedEvent();
	float prevHealth = health;
	health += amount;
	Clamp(0, maxHealth, &health);
	if (health == 0) DeathEvent();

	healthMutex.unlock();
	return health - prevHealth;
}
const bool Health::IsAlive() const {
	return health > 0;
}

void Health::MakeInvincible() {
	health = 0;
}