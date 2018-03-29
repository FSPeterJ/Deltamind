#include "Gun.h"
#include "Projectile.h"
#include "PhysicsManager.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "Console.h"
#include "Wwise_IDs.h"
#include "TextManager.h"

void Gun::Overheat::CreateBar(Gun* _parent) {
	parent = _parent;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ProgressBar>(overheatBarPID, { 0.0f, 0.0f, 0.0f }, &bar)); // stop using magic number prefab ID
	bar->ToggleFlag(GAMEOBJECT_PUBLIC_FLAGS::UNLIT);
	bar->Enable();
	bar->PersistOnReset();
}
bool Gun::Overheat::CanShoot(float fireRate) {
	return timeSinceLastShot > (1 / fireRate) && !energyOverheatDelayTimeLeft;
}
bool Gun::Overheat::AddEnergy(float energy) {
	currentEnergy += energy;
	if(currentEnergy >= energyLimit) {
		currentEnergy = energyLimit;
		energyOverheatDelayTimeLeft = energyOverheatDelay;
		return false;
	}
	return true;
}
void Gun::Overheat::Update(bool active) {
	//Update Overheat Stats
	float dt = (float)GhostTime::DeltaTime();
	timeSinceLastShot += dt;
	if(energyOverheatDelayTimeLeft > 0) {
		energyOverheatDelayTimeLeft -= dt;
		if(energyOverheatDelayTimeLeft <= 0) {
			energyOverheatDelayTimeLeft = 0;
			currentEnergy = 0;
		}
	}
	else if(timeSinceLastShot >= energyWaitCooldown) {
		currentEnergy = 0;
	}

	// This seems weird but I have no better ideas at the moment
	if(active) {
		//Update Bar itself
		DirectX::XMFLOAT4X4 newPos;
		newPos = parent->transform.GetMatrix();
		bar->transform.NormalizeAllAxis();
		newPos._41 -= bar->transform.GetMatrix()._11 * 0.04f;
		newPos._42 -= bar->transform.GetMatrix()._12 * 0.04f;
		newPos._43 -= bar->transform.GetMatrix()._13 * 0.04f;
		
		newPos._41 -= bar->transform.GetMatrix()._31 * 0.09f;
		newPos._42 -= bar->transform.GetMatrix()._32 * 0.09f;
		newPos._43 -= bar->transform.GetMatrix()._33 * 0.09f;

		bar->transform.SetMatrix(newPos);

		bar->SetBarPercentage(currentEnergy / energyLimit);
	}
	else bar->SetBarPercentage(0);


	//Console Write
	//Console::WriteLine << currentEnergy << "__" << energyLimit << " : " << energyOverheatDelayTimeLeft << " : " << timeSinceLastShot;
}

Gun::Gun() {
	SetTag("Gun");
}

void Gun::Awake(Object* obj) {
	state = GUN;
	Gun* gun = ((Gun*)obj);
	overheat.overheatBarPID = gun->overheat.overheatBarPID;
	projectiePID = gun->projectiePID;
	fireRate = gun->fireRate;
	damage = gun->damage;
	type = gun->type;
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
	GameObject::Awake(obj);
	//TextManager::textOutput out = TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "This is a test!");
	//Material* newMat = TextManager::CreateRenderableTexture(100, 100);
	//TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", "This is a test!", newMat);
	//TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", "This is a test!", GetComponent<Material>());
	//SetComponent<Material>(out.mat);
}

void Gun::GivePID(unsigned pid, const char* tag) {
	// Look into a better system
	if(!strcmp(tag, "projectile") ) {
		projectiePID = pid;
	}
	else if(!strcmp(tag, "overheat")) {
		overheat.overheatBarPID = pid;
	}
}

bool Gun::Shoot(bool addOverheat) {
	// What does this switch statement do???
	switch(type) {
		case AUTO:
			if(!addOverheat || overheat.CanShoot(fireRate)) {
				//Fire
				Projectile* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePID, { 0, 0, 0 }, &obj));
				MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_WEN));
				DirectX::XMFLOAT4X4 newPos;
				newPos = transform.GetMatrix();
				newPos._41 += newPos._31 * 0.4f;
				newPos._42 += newPos._32 * 0.4f;
				newPos._43 += newPos._33 * 0.4f;
				obj->transform.SetMatrix(newPos);
				flash.SetAsPoint({ 0.0f, 0.0f, 11.0f }, { newPos._41, newPos._42, newPos._43 }, 1.2f);
				flash.SetTimed(0.1);
				flash.Enable();
				obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
				obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(transform.GetMatrix()._31 * 50.0f, transform.GetMatrix()._32 * 50.0f, transform.GetMatrix()._33 * 50.0f);
				obj->SetDamage(damage);
				obj->Enable();
				if (addOverheat) {
					overheat.AddEnergy(overheat.energyBulletCost);
					overheat.ResetTimeSinceLastShot();
				}
			}
			break;
		case SEMI:
			if(!addOverheat || overheat.CanShoot(fireRate)) {
				//Fire
				Projectile* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePID, { 0, 0, 0 }, &obj));
				MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_WEN));
				DirectX::XMFLOAT4X4 newPos;
				newPos = transform.GetMatrix();
				newPos._41 += newPos._31 * 0.4f;
				newPos._42 += newPos._32 * 0.4f;
				newPos._43 += newPos._33 * 0.4f;
				obj->transform.SetMatrix(newPos);
				PhysicsComponent* temp2 = obj->GetComponent<PhysicsComponent>();
				RigidBody* temp = &temp2->rigidBody;
				temp->AdjustGravityMagnitude(0);
				flash.SetAsPoint({ 0.0f, 0.0f, 11.0f }, { newPos._41, newPos._42, newPos._43 }, 1.2f);
				flash.SetTimed(0.1);
				flash.Enable();
				obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(transform.GetMatrix()._31 * 50.0f, transform.GetMatrix()._32 * 50.0f, transform.GetMatrix()._33 * 50.0f);
				obj->SetDamage(damage);
				obj->Enable();
				if (addOverheat) {
					overheat.AddEnergy(overheat.energyBulletCost);
					overheat.ResetTimeSinceLastShot();
				}
				return false;
			}
			break;
	}
	return true;
}
void Gun::InactiveUpdate() {
	overheat.Update(false);
}
void Gun::ActiveUpdate() {
	overheat.Update(true);
	DirectX::XMFLOAT4X4 pos = transform.GetMatrix();
	pos._41 += pos._31 * 0.4f;
	pos._42 += pos._32 * 0.4f;
	pos._43 += pos._33 * 0.4f;
	flash.transform.SetMatrix(pos);
}

void Gun::Selected() {
	Item::Selected();
	overheat.bar->Render();
}
void Gun::DeSelected() {
	Item::DeSelected();
	overheat.bar->UnRender();
}

#ifdef _DEBUG
void Gun::SmokeTest() {
	assert(projectiePID != 0);
	assert(overheat.overheatBarPID != 0);
}
#endif


