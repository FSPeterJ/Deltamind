#include "Projectile.h"
#include "GhostTime.h"
#include "MessageEvents.h"

Projectile::Projectile() {
	SetTag("Bullet");
}

Projectile::~Projectile() {}

void Projectile::Update() {
	timeSinceShot += (float) GhostTime::DeltaTime();
	//This should be a parameter that makes it way all the way from main or some other collection, don't query per object per frame

	if(timeSinceShot > maxProjectileTime) {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
		timeSinceShot = 0;
	}
}

void Projectile::OnCollision(GameObject* object) {
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });

	timeSinceShot = 0;
}

void Projectile::SetDamage(float _damage) {
	damage = _damage;
}