#include "Gun.h"
#include "Projectile.h"
#include "PhysicsManager.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "Console.h"
#include "Wwise_IDs.h"



void Gun::Overheat::CreateBar() {
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ProgressBar>(overheatBarPID, { 0.0f, 0.0f, 0.0f }, &bar)); // stop using magic number prefab ID
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
		newPos = parent->GetPosition();
		newPos._41 -= bar->GetPosition()._11 * 0.04f;
		newPos._42 -= bar->GetPosition()._12 * 0.04f;
		newPos._43 -= bar->GetPosition()._13 * 0.04f;

		newPos._41 -= bar->GetPosition()._31 * 0.1f;
		newPos._42 -= bar->GetPosition()._32 * 0.1f;
		newPos._43 -= bar->GetPosition()._33 * 0.1f;

		bar->SetPosition(newPos);

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
	overheat.CreateBar(); // may want to reethink this?
	projectiePID = gun->projectiePID;
	fireRate = gun->fireRate;
	damage = gun->damage;
	type = gun->type;
	overheat.parent = this;
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
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

bool Gun::Shoot() {

	// What does this switch statement do???
	switch(type) {
		case AUTO:
			if(overheat.CanShoot(fireRate)) {
				//Fire
				Projectile* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePID, { 0, 0, 0 }, &obj));
				MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_WEN));
				DirectX::XMFLOAT4X4 newPos;
				newPos = GetPosition();
				newPos._41 += newPos._31 * 0.2f;
				newPos._42 += newPos._32 * 0.2f;
				newPos._43 += newPos._33 * 0.2f;
				obj->SetPosition(newPos);
				obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
				obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(GetPosition()._31 * 10.0f, GetPosition()._32 * 10.0f, GetPosition()._33 * 10.0f);
				obj->SetDamage(damage);
				obj->Enable();
				overheat.AddEnergy(overheat.energyBulletCost);
				overheat.ResetTimeSinceLastShot();
			}
			break;
		case SEMI:
			if(overheat.CanShoot(fireRate)) {
				//Fire
				Projectile* obj;
				MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<Projectile>(projectiePID, { 0, 0, 0 }, &obj));
				MessageEvents::SendMessage(EVENT_RequestSound, SoundRequestMessage(this, AK::EVENTS::PLAY_WEN));
				DirectX::XMFLOAT4X4 newPos;
				newPos = GetPosition();
				newPos._41 += newPos._31 * 0.2f;
				newPos._42 += newPos._32 * 0.2f;
				newPos._43 += newPos._33 * 0.2f;
				obj->SetPosition(newPos);
				PhysicsComponent* temp2 = obj->GetComponent<PhysicsComponent>();
				RigidBody* temp = &temp2->rigidBody;
				temp->AdjustGravityMagnitude(0);
				obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(GetPosition()._31 * 10.0f, GetPosition()._32 * 10.0f, GetPosition()._33 * 10.0f);
				obj->SetDamage(damage);
				obj->Enable();
				overheat.AddEnergy(overheat.energyBulletCost);
				overheat.ResetTimeSinceLastShot();
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
}



#ifdef _DEBUG
void Gun::SmokeTest() {
	assert(projectiePID != 0);
	assert(overheat.overheatBarPID != 0);
}
#endif


