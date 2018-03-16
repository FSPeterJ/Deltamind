#include "Projectile.h"
#include "GhostTime.h"
#include "MessageEvents.h"

Projectile::Projectile() {
	SetTag("Bullet");
}

Projectile::~Projectile() {}

void Projectile::Awake(Object* obj) {
	++instantiatedCount;
	GameObject::Awake(obj);
}

void Projectile::Update() {
	timeSinceShot += (float)GhostTime::DeltaTime();

	if (timeSinceShot > maxProjectileTime) {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
		timeSinceShot = 0;
	}
}

void Projectile::OnCollision(GameObject* object) {

	if (object->GetTag() != std::string("Turret"))
	{
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	}
	else {
		throw "hit a turret";
	}

	timeSinceShot = 0;
}

void Projectile::SetDamage(float _damage) {
	damage = _damage;
}

void Projectile::Destroy() {
	++destroyedCount;
	GameObject::Destroy();
}