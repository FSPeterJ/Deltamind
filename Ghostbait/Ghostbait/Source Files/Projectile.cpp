#include "Projectile.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"
#include "Wwise_IDs.h"
#include "Emitter.h"
#include "DirectXMath.h"

using namespace DirectX;

Projectile::Projectile() {
	SetTag("Bullet");
}

Projectile::~Projectile() {}

void Projectile::Awake(Object* obj) {
	++instantiatedCount;
	isDestroying = false;
	GameObject::Awake(obj);
	pc = GetComponent<PhysicsComponent>();
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, ObjectMessage(this));
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
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_SFX_BULLETHIT));

	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });

	Emitter* emitter = nullptr;

	DirectX::XMFLOAT3 force;
	DirectX::XMVECTOR forceV = GetComponent<PhysicsComponent>()->rigidBody.GetVelocity();
	forceV = forceV  * 0.1f;
	forceV = DirectX::XMVectorNegate(forceV);
	forceV = XMVectorClamp(forceV, { -2,-2,-2 }, {2,2,2});

	DirectX::XMStoreFloat3(&force, forceV);
	MessageEvents::SendMessage(EVENT_NewEmitter, NewEmitterMessage(&transform.GetPosition(), 0, force, (ComponentBase**)&emitter));
	emitter->parentObject = this;
	emitter->Enable();


	isDestroying = true;
	gameObjMutex.unlock();
}

void Projectile::SetDamage(float _damage) {
	damage = _damage;
}

void Projectile::Destroy() {
	++destroyedCount;
	MessageEvents::SendMessage(EVENT_UnregisterNoisemaker, ObjectMessage(this));
	GameObject::Destroy();
}