
#include "Turret.h"

#include <DirectXMath.h>
#include "DebugRenderer.h"
#include "Animator.h"
#include "Projectile.h"
#include "MessageEvents.h"
#include "Wwise_IDs.h"
#include "PhysicsComponent.h"
#include "GhostTime.h"


Turret::Turret() {
	tag = std::string("Turret");
}

void Turret::Enable() {
	eventDestroy = MessageEvents::Subscribe(EVENT_Destroy, [=](EventMessageBase* e) {
		if(target == ((StandardObjectMessage*)e)->RetrieveObject()) {
			this->target = nullptr;
			targetDistance = 99999;
		}
	});
	GameObject::Enable();
}

void Turret::Disable() {
	MessageEvents::UnSubscribe(EVENT_Destroy, eventDestroy);
	GameObject::Disable();
}

void Turret::Awake() {
	targetDistance = 9999999;
	target = nullptr;
	firerate = 2;
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
	turretPitch = GetComponent<Animator>()->getJointByName("RocketLauncher_DeployedSetup:Pitch");
}

void Turret::Update() {
	float dt = (float)GhostTime::DeltaTime();
	timeSinceLastShot += dt;
	if(target != nullptr) {
		using namespace DirectX;
		//XMVECTOR jointoffset = XMLoadFloat3(&(XMFLOAT3)turretPitch->m[3]);
		XMVECTOR jointoffset = { 0,1.0f,0 };
		XMVECTOR pos = XMLoadFloat3(&(XMFLOAT3)position.m[3]);
		pos += jointoffset;
		XMFLOAT3 newpos;
		XMStoreFloat3(&newpos, pos);
		DebugRenderer::AddLine(newpos, (XMFLOAT3)target->position.m[3], DirectX::XMFLOAT3(1.0f, 0.6f, 0.0f));

		targetDistance = CalculateDistance(target);
		if(CanShoot(firerate)) {
			Shoot();
		}
		if(targetDistance > 5) {
			target = nullptr;
			targetDistance = 99999;

		}

	}

}

float Turret::CalculateDistance(GameObject* obj) {
	float length;
	using namespace DirectX;
	//XMVECTOR jointoffset = XMLoadFloat3(&(XMFLOAT3)turretPitch->m[3]);
	XMVECTOR jointoffset = { 0,1.0f,0 };
	XMVECTOR pos = XMLoadFloat3(&(XMFLOAT3)position.m[3]);
	pos += jointoffset;
	XMVECTOR enemypos = XMLoadFloat3(&(XMFLOAT3)obj->position.m[3]);
	pos = DirectX::XMVector3Length(pos - enemypos);
	XMVectorAbs(pos);
	XMStoreFloat(&length, pos);
	return length;
}

Turret::~Turret() {
}

void Turret::OnTrigger(GameObject* object) {
	if(!strcmp(object->GetTag().c_str(), "Enemy") || !strcmp(object->GetTag().c_str(), "PhysicsTestObj")) {
		using namespace DirectX;
		float length = CalculateDistance(object);;
		if(length < targetDistance) {
			target = object;
			targetDistance = length;
		}
	}
}

bool Turret::CanShoot(float fireRate) {
	return timeSinceLastShot >(1 / fireRate);
}

void Turret::Shoot() {
	//Fire
	using namespace DirectX;

	Projectile* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePID, { 0, 0, 0 }, &obj));
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_WEN));
	obj->position = position;
	obj->position._42 += 1.0f;
	//obj->position._41 += obj->position._31 * 0.2f;
	//obj->position._42 += obj->position._32 * 0.2f;
	//obj->position._43 += obj->position._33 * 0.2f;
	XMVECTOR bulletpos = DirectX::XMLoadFloat4(&(XMFLOAT4)obj->position.m[3]);
	XMVECTOR targetPos = DirectX::XMLoadFloat4(&(XMFLOAT4)target->position.m[3]);
	XMVECTOR Z(XMVector3Normalize(targetPos - bulletpos));
	XMVECTOR X(XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 1), Z)));
	XMVECTOR Y(XMVector3Normalize(XMVector3Cross(Z, X)));
	XMMATRIX lookat(
		X,
		Y,
		Z,
		bulletpos
	);
	//lookat =  lookat * DirectX::XMMatrixTranslationFromVector(bulletpos);
	DirectX::XMStoreFloat4x4(&obj->position, lookat);

	PhysicsComponent* temp2 = obj->GetComponent<PhysicsComponent>();
	RigidBody* temp = &temp2->rigidBody;
	temp->AdjustGravityMagnitude(0);
	obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(obj->position._31 * 10.0f, obj->position._32 * 10.0f, obj->position._33 * 10.0f);
	obj->SetDamage(damage);
	obj->Enable();
	timeSinceLastShot = (float)GhostTime::DeltaTime();
}

void Turret::CloneData(Object* obj) {
	projectiePID = ((Turret*)obj)->projectiePID;

}

void Turret::GivePID(unsigned pid, const char* tag) {
	// Look into a better system
	if(!strcmp(tag, "projectile")) {
		projectiePID = pid;
	}
}