
#include "Turret.h"

#include <DirectXMath.h>
#include "DebugRenderer.h"
#include "Animator.h"
#include "Projectile.h"
#include "MessageEvents.h"
#include "Wwise_IDs.h"
#include "PhysicsComponent.h"
#include "GhostTime.h"



#include "Material.h"
#include "MeshManager.h"

Turret::Turret() {
	tag = std::string("Turret");
}
void Turret::Enable() {
	if (!eventDestroy) {
		eventDestroy = MessageEvents::Subscribe(EVENT_Destroy, [=](EventMessageBase* e) {
			if (target == ((StandardObjectMessage*)e)->RetrieveObject()) {
				this->target = nullptr;
				targetDistance = 99999;
			}
		});
	}
	GameObject::Enable();
}
void Turret::Disable() {
	if (eventDestroy) {
		MessageEvents::UnSubscribe(EVENT_Destroy, eventDestroy);
	}
	GameObject::Disable();
}
void Turret::Awake(Object* obj) {
	Turret* turret = ((Turret*)obj);
	isDestroyed = false;
	eventDestroy = 0;
	projectiePID = turret->projectiePID;
	targetDistance = 9999999;
	target = nullptr;
	firerate = turret->firerate;
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
	anim = GetComponent<Animator>();
	turretPitch = anim->getJointByName("Pitch");
	turretYaw = anim->getJointByName("Yaw");
	assert(turretPitch);
	launcherorigin = anim->getJointByName("Launcher_1");
	assert(launcherorigin);
	GameObject::Awake(obj);
}
void Turret::Update() {
	float dt = (float)GhostTime::DeltaTime();


	timeSinceLastShot += dt;
	if(target != nullptr) {
		hasTargeted = true;
		using namespace DirectX;
		////XMVECTOR jointoffset = XMLoadFloat3(&(XMFLOAT3)turretPitch->m[3]);
		//XMVECTOR jointoffset = { 0,1.0f,0 };
		XMVECTOR pos = XMLoadFloat3(&(XMFLOAT3)transform.matrix.m[3]); //This can probably be cached
		////pos += jointoffset;
		//XMFLOAT3 newpos;
		//XMStoreFloat3(&newpos, pos);
		XMVECTOR XMturretYaw = DirectX::XMLoadFloat4(&(XMFLOAT4)turretYaw->m[3]);
		//XMturretYaw += pos;
		XMVECTOR XMturretPitch = DirectX::XMLoadFloat4(&(XMFLOAT4)turretPitch->m[3]);
		//XMturretPitch += pos;
		XMVECTOR targetPos = DirectX::XMLoadFloat4(&(XMFLOAT4)target->transform.GetMatrix().m[3]); //+ XMVectorSet(0,1,0,0);
		targetPos -= pos;
		targetPos += XMVectorSet(0,1,0,0); // so they don't shoot the floor
		//DebugRenderer::AddLine((XMFLOAT3)turretPitch->m[3], (XMFLOAT3)target->transform.GetMatrix().m[3], { 0,0.6f, 0.2f });

		XMVECTOR mX(XMVector3Normalize(XMVector3Cross(XMVectorSet(0, 1, 0, 0), XMVector3Normalize(targetPos - XMturretYaw))));
		XMVECTOR mZ(XMVector3Normalize(XMVector3Cross(mX, XMVectorSet(0, 1, 0, 0))));
		XMVECTOR mY(XMVector3Normalize(XMVector3Cross(mZ, mX)));
		XMMATRIX lookatYaw(
			mX,
			mY,
			mZ,
			XMVectorSet(0, 0, 0, 1)
		);


		//XMVECTOR Z(XMVector3Normalize(XMVector3Cross(XMVectorSet(1, 0, 0, 0), XMVector3Normalize(targetPos - XMturretPitch))));
		//XMVECTOR Y(XMVector3Normalize(XMVector3Cross(Z , XMVectorSet(1, 0, 0, 0))));
		//XMVECTOR X(XMVector3Normalize(XMVector3Cross(Y, Z)));
		//XMMATRIX lookatPitch(
		//	X,
		//	Y,
		//	Z,
		//	XMVectorSet(0, 0, 0, 1)
		//);



		XMVECTOR Y(XMVector3Normalize(XMVector3Cross(XMVector3Normalize(XMVector3TransformNormal(XMVector3Normalize(targetPos - XMturretPitch), XMMatrixInverse(nullptr, lookatYaw))), XMVectorSet(1, 0, 0, 0)))); // forces pitch
		XMVECTOR X(XMVector3Normalize(XMVector3Cross(Y, XMVectorSet(0, 0, 1, 0))));
		XMVECTOR Z(XMVector3Normalize(XMVector3Cross(X, Y)));
		XMMATRIX lookatPitch(
			X,
			Y,
			Z,
			XMVectorSet(0, 0, 0, 1)
		);

		//XMVECTOR X(XMVector3Normalize(XMVector3Cross(XMVector3Normalize(targetPos - XMturretPitch), XMVectorSet(1, 0, 0, 0))));
		//XMVECTOR Z(XMVector3Normalize(XMVector3Cross(X, XMVectorSet(1, 0, 0, 0))));
		//XMVECTOR Y(XMVector3Normalize(XMVector3Cross(Z, X)));
		//XMMATRIX lookatPitch(
		//	X,
		//	Y,
		//	Z,
		//	XMVectorSet(0, 0, 0, 1)
		//);


		anim->ManipulateJointByName("Pitch", lookatPitch);
		anim->ManipulateJointByName("Yaw", lookatYaw);
		DirectX::XMStoreFloat4x4(&prevPitch, lookatPitch);
		DirectX::XMStoreFloat4x4(&prevYaw, lookatYaw);
		

		//lookat =  lookat * DirectX::XMMatrixTranslationFromVector(bulletpos);
		//DirectX::XMStoreFloat4x4(turretPitch, lookat);

		DirectX::XMFLOAT4X4 newPos = *launcherorigin;

		newPos._41 += transform.matrix._41;
		newPos._42 += transform.matrix._42;
		newPos._43 += transform.matrix._43;
		DebugRenderer::DrawAxes(newPos, 1);
		newPos = *turretYaw;
		newPos._41 += transform.matrix._41;
		newPos._42 += transform.matrix._42;
		newPos._43 += transform.matrix._43;
		DebugRenderer::DrawAxes(newPos, 1);
		newPos = *turretPitch;
		newPos._41 += transform.matrix._41;
		newPos._42 += transform.matrix._42;
		newPos._43 += transform.matrix._43;
		DebugRenderer::DrawAxes(newPos, 1);

		targetDistance = CalculateDistance(target);
		if(CanShoot(firerate)) {
			Shoot();
		}
		if(targetDistance > 5) {
			target = nullptr;
			targetDistance = 99999;
		}
		
	}
	else if(hasTargeted) {
		anim->ManipulateJointByName("Pitch", DirectX::XMLoadFloat4x4(&prevPitch));
		anim->ManipulateJointByName("Yaw", DirectX::XMLoadFloat4x4(&prevYaw));
	}
}
float Turret::CalculateDistance(GameObject* obj) {
	float length;
	using namespace DirectX;
	//XMVECTOR jointoffset = XMLoadFloat3(&(XMFLOAT3)turretPitch->m[3]);
	XMVECTOR jointoffset = { 0,1.0f,0 };
	XMVECTOR pos = XMLoadFloat3(&(XMFLOAT3)transform.GetMatrix().m[3]);
	pos += jointoffset;
	XMVECTOR enemypos = XMLoadFloat3(&(XMFLOAT3)obj->transform.GetMatrix().m[3]);
	pos = DirectX::XMVector3Length(pos - enemypos);
	XMVectorAbs(pos);
	XMStoreFloat(&length, pos);
	return length;
}
Turret::~Turret() {
}
void Turret::DeathEvent() {
	gameObjMutex.lock();
	if (!isDestroyed) {
		isDestroyed = true;
		Destroy();
	}
	gameObjMutex.unlock();
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
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_SFX_GUNSHOT));
	DirectX::XMFLOAT4X4 newPos = *launcherorigin;

	newPos._41 += transform.matrix._41;
	newPos._42 += transform.matrix._42;
	newPos._43 += transform.matrix._43;
	obj->transform.SetMatrix(newPos);
	PhysicsComponent* pc = obj->GetComponent<PhysicsComponent>();
	pc->rigidBody.AdjustGravityMagnitude(0);

	//why arent we using 
	//temp->SetVelocity(obj->position._31 * 10.0f, obj->position._32 * 10.0f, obj->position._33 * 10.0f);
	pc->rigidBody.SetVelocity(launcherorigin->_31 * 35.0f, launcherorigin->_32 * 35.0f, launcherorigin->_33 * 35.0f);
	obj->SetDamage(damage);
	obj->Enable();
	timeSinceLastShot = (float)GhostTime::DeltaTime();
}
void Turret::Destroy() {
	MessageEvents::SendMessage(EVENT_RemoveObstacle, SnapMessage(&DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43)));
	GameObject::Destroy();
}
void Turret::GivePID(unsigned pid, const char* tag) {
	// Look into a better system
	if(!strcmp(tag, "projectile")) {
		projectiePID = pid;
	}
}

void Turret_Long::Awake(Object* obj) {
	Turret::Awake(obj);
	firerate = 0.5f;
	damage = 100;
	buildCost = 500;
}
void Turret_Long::Shoot() {
	//Fire
	using namespace DirectX;

	Projectile* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePID, { 0, 0, 0 }, &obj));
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_SFX_TURRET_SINGLE_SHOT));
	DirectX::XMFLOAT4X4 newPos = *launcherorigin;
	float dy = 0.2f;
	float dz = 0.8f;
	newPos._41 += transform.matrix._41 + (newPos._21 * dy) + (newPos._31 * dz);
	newPos._42 += transform.matrix._42 + (newPos._22 * dy) + (newPos._32 * dz);
	newPos._43 += transform.matrix._43 + (newPos._23 * dy) + (newPos._33 * dz);
	obj->transform.SetMatrix(newPos);
	obj->SwapComponentVarient<Material>("Red");
	PhysicsComponent* pc = obj->GetComponent<PhysicsComponent>();
	pc->rigidBody.AdjustGravityMagnitude(0);

	//why arent we using 
	//temp->SetVelocity(obj->position._31 * 10.0f, obj->position._32 * 10.0f, obj->position._33 * 10.0f);
	pc->rigidBody.SetVelocity(launcherorigin->_31 * 35.0f, launcherorigin->_32 * 35.0f, launcherorigin->_33 * 35.0f);
	obj->SetDamage(damage);
	obj->Enable();
	timeSinceLastShot = (float)GhostTime::DeltaTime();
}
void Turret_Medium::Awake(Object* obj) {
	Turret::Awake(obj);
	firerate = 3;
	damage = 10;
	buildCost = 400;
}
void Turret_Medium::Shoot() {
	//Fire
	using namespace DirectX;

	Projectile* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePID, { 0, 0, 0 }, &obj));
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_SFX_PLASMADECAYFIZZLE));
	DirectX::XMFLOAT4X4 newPos = *launcherorigin;
	float dy = 0.3f;
	float dz = 0.8f;
	newPos._41 += transform.matrix._41 + (newPos._21 * dy) + (newPos._31 * dz);
	newPos._42 += transform.matrix._42 + (newPos._22 * dy) + (newPos._32 * dz);
	newPos._43 += transform.matrix._43 + (newPos._23 * dy) + (newPos._33 * dz);
	obj->transform.SetMatrix(newPos);
	obj->SwapComponentVarient<Material>("Green");
	PhysicsComponent* pc = obj->GetComponent<PhysicsComponent>();
	pc->rigidBody.AdjustGravityMagnitude(0);

	//why arent we using 
	//temp->SetVelocity(obj->position._31 * 10.0f, obj->position._32 * 10.0f, obj->position._33 * 10.0f);
	pc->rigidBody.SetVelocity(launcherorigin->_31 * 35.0f, launcherorigin->_32 * 35.0f, launcherorigin->_33 * 35.0f);
	obj->SetDamage(damage);
	obj->Enable();
	timeSinceLastShot = (float)GhostTime::DeltaTime();
}
void Turret_Short::Awake(Object* obj) {
	Turret::Awake(obj);
	firerate = 3.25F;
	damage = 3;
	buildCost = 350;
}
void Turret_Short::Shoot() {
	//Fire
	using namespace DirectX;

	Projectile* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePID, { 0, 0, 0 }, &obj));
	MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_SFX_LAZER_COMPUTERMADE));
	DirectX::XMFLOAT4X4 newPos = *launcherorigin;

	newPos._41 += transform.matrix._41 + (newPos._11 * offset[currentIndex].dx) + (newPos._21 * offset[currentIndex].dy) + (newPos._31 * offset[currentIndex].dz);
	newPos._42 += transform.matrix._42 + (newPos._12 * offset[currentIndex].dx) + (newPos._22 * offset[currentIndex].dy) + (newPos._32 * offset[currentIndex].dz);
	newPos._43 += transform.matrix._43 + (newPos._13 * offset[currentIndex].dx) + (newPos._23 * offset[currentIndex].dy) + (newPos._33 * offset[currentIndex].dz);
	++currentIndex;
	if (currentIndex >= 4) {
		currentIndex = 0;
	}
	obj->transform.SetMatrix(newPos);
	obj->SwapComponentVarient<Material>("Blue");
	PhysicsComponent* pc = obj->GetComponent<PhysicsComponent>();
	pc->rigidBody.AdjustGravityMagnitude(0);

	//why arent we using 
	pc->rigidBody.SetVelocity(launcherorigin->_31 * 35.0f, launcherorigin->_32 * 35.0f, launcherorigin->_33 * 35.0f);
	obj->SetDamage(damage);
	obj->Enable();
	timeSinceLastShot = (float)GhostTime::DeltaTime();
}