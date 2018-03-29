#include "Health.h"

#define default_health 100

Health::Health() {
	maxHealth = default_health;
	health = default_health;
}
Health::Health(float _maxHealth) {
	maxHealth = _maxHealth;
	health = _maxHealth;
}
Health::Health(float _maxHealth, float _startingHealth) {
	maxHealth = _maxHealth;
	health = _startingHealth;
	Clamp(0, maxHealth, &health);
}

void Health::Clamp(float min, float max, float* val) {
	if (*val < min) *val = min;
	else if (*val > max) *val = max;
}

float Health::PercentHealth() {
	return (health / maxHealth);
}
float Health::AdjustHealth(float amount) {
	float prevHealth = health;
	if (amount < 0) HurtEvent();
	if (amount > 0) HealedEvent();
	health += amount;
	Clamp(0, maxHealth, &health);
	if (health == 0) DeathEvent();
	return health - prevHealth;
}
bool Health::IsAlive() {
	if (health <= 0) return false;
	return true;
}