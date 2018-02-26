#include "Health.h"

#define default_health 1

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
void Health::AdjustHealth(float amount) {
	health += amount;
	Clamp(0, maxHealth, &health);
}
bool Health::IsAlive() {
	if (health <= 0) return false;
	return true;
}