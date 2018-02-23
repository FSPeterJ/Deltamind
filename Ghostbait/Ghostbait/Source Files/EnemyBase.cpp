#include "EnemyBase.h"
#include "GhostTime.h"
#include "PhysicsComponent.h"
#include "MessageEvents.h"
#include "Console.h"
#include "Projectile.h"
#include "Material.h"



void EnemyBase::Awake() {
	eventLose = MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) {this->RestartGame(); });
}

void EnemyBase::Disable() {
	MessageEvents::UnSubscribe(EVENT_GameLose, eventLose);
	GameObject::Disable();
}

void EnemyBase::RestartGame() {
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
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

void EnemyBase::OnCollision(GameObject* _other) {
	PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();
	DirectX::XMVECTOR incomingDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat4x4(&position).r[3], DirectX::XMLoadFloat4x4(&(_other->position)).r[3]));
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
}

void EnemyBase::CloneData(Object* obj) {
	componentVarients = obj->componentVarients;
}

void EnemyBase::Destroy() {
	MessageEvents::SendMessage(EVENT_EnemyDied, EventMessageBase());
	GameObject::Destroy();
}