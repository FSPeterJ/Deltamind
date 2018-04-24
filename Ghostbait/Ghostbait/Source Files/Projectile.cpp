#include "Projectile.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"

Projectile::Projectile() {
	SetTag("Bullet");
}

Projectile::~Projectile() {}

void Projectile::Awake(Object* obj) {
	++instantiatedCount;
	isDestroying = false;
	GameObject::Awake(obj);
	pc = GetComponent<PhysicsComponent>();
}

void Projectile::Update() {
	timeSinceShot += (float)GhostTime::DeltaTime();
	
	if (DirectX::XMVectorGetX(DirectX::XMVector3Length(pc->rigidBody.GetVelocity())) <= 5) {
		int i = 0;
	}

	if (timeSinceShot > maxProjectileTime) {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
		timeSinceShot = 0;
	}
}

void Projectile::OnCollision(GameObject* object) {
	gameObjMutex.lock();
	if (isDestroying || strcmp(object->GetTag().c_str(), "Turret") == 0 || strcmp(object->GetTag().c_str(), "Bullet") == 0) {
		gameObjMutex.unlock();
		return;
	}
	
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	isDestroying = true;
	gameObjMutex.unlock();
}

void Projectile::SetDamage(float _damage) {
	damage = _damage;
}

void Projectile::Destroy() {
	++destroyedCount;
	GameObject::Destroy();
}