#include "EnemyBase.h"
#include "GhostTime.h"
#include "PhysicsComponent.h"
#include "MessageEvents.h"
#include "Console.h"
#include "Projectile.h"
#include "Material.h"



void EnemyBase::Awake(Object* obj) {
	currState = IDLE;
	maxSpeed = (float)((rand() % 3) + 1);
	target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	hurt = false;
	hurtTimer = 0;
	hurtDuration = 1;
	sentDeathMessage = false;

	eventLose = 0;
	SetToFullHealth();
	GameObject::Awake(obj);
}
void EnemyBase::Subscribe() {
	if(!eventLose) eventLose = MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) { MessageEvents::SendQueueMessage(EVENT_Late, [=] {this->Destroy(); }); });
}
void EnemyBase::UnSubscribe() {
	if (eventLose) {
		MessageEvents::UnSubscribe(EVENT_GameLose, eventLose); 
		eventLose = 0;
	}
}
void EnemyBase::Enable() {
	EnemyBase::Subscribe();
	GameObject::Enable();
}
void EnemyBase::Disable() {
	EnemyBase::UnSubscribe();
	GameObject::Disable();
}
void EnemyBase::Destroy() {
	MessageEvents::SendMessage(EVENT_EnemyDied, EventMessageBase());
	GameObject::Destroy();
}
void EnemyBase::Update() {
	if (hurt) {
		hurtTimer += GhostTime::DeltaTime();
		if (hurtTimer >= 0.25f) {
			hurt = false;
			int id = TypeMap::GetComponentTypeID<Material>();
			SetComponent(defaultMat, id);
		}
	}
	GameObject::Update();
	//DirectX::XMVECTOR directionToGoal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat4x4(&position).r[3]));
	//PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();
	//
	//myPhys->rigidBody.AddForce(0.2f * (float) GhostTime::DeltaTime(), DirectX::XMVectorGetX(directionToGoal), DirectX::XMVectorGetY(directionToGoal), DirectX::XMVectorGetZ(directionToGoal));
	//
	//if(myPhys->rigidBody.GetSpeedSq() > maxSpeed * maxSpeed) {
	//	DirectX::XMVECTOR clampedVelocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(myPhys->rigidBody.GetVelocity()), maxSpeed);
	//	myPhys->rigidBody.SetVelocity(clampedVelocity);
	//}
}

//Other Overrides
void EnemyBase::OnCollision(GameObject* _other) {
	PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();
	DirectX::XMVECTOR incomingDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat4x4(&transform.GetMatrix()).r[3], DirectX::XMLoadFloat4x4(&(_other->transform.GetMatrix())).r[3]));
	if(_other->GetTag() == "Bullet") {
		myPhys->rigidBody.AddForce(0.2f, DirectX::XMVectorGetX(incomingDirection), 0.0f, DirectX::XMVectorGetZ(incomingDirection));
		AdjustHealth(-(((Projectile*)_other)->damage));
		if (componentVarients.find("Hurt") != componentVarients.end()) {
			if (!hurt) {
				int id = TypeMap::GetComponentTypeID<Material>();
				defaultMat = GetComponent<Material>();
				SetComponent(componentVarients["Hurt"], id);
			}
			hurt = true;
			hurtTimer = 0;
		}
		if(!IsAlive() && !sentDeathMessage) {
			//Destroy itself
			//if(temp > 3) {
			//	MessageEvents::SendMessage(EVENT_GameWin, EventMessageBase());
			//	Console::WriteLine << "GAME WAS WON";
			//	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage(9/*WinCube*/, {0, 0.75f, 0}));
			//}
			sentDeathMessage = true;
			MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
		}
	}

	GameObject::OnCollision(_other);
}
void EnemyBase::RandomizeStats() {

}