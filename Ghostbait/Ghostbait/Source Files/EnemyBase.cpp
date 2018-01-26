#include "EnemyBase.h"
#include "GhostTime.h"
#include "PhysicsComponent.h"
#include "Console.h"

void EnemyBase::Update() {
	DirectX::XMVECTOR directionToGoal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat4x4(&position).r[3]));
	PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();

	myPhys->rigidBody.AddForce(0.2f * (float)GhostTime::DeltaTime(), DirectX::XMVectorGetX(directionToGoal), DirectX::XMVectorGetY(directionToGoal), DirectX::XMVectorGetZ(directionToGoal));

	if(myPhys->rigidBody.GetSpeedSq() > maxSpeed * maxSpeed) {
		DirectX::XMVECTOR clampedVelocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(myPhys->rigidBody.GetVelocity()), maxSpeed);
		myPhys->rigidBody.SetVelocity(clampedVelocity);
	}
}

static int temp = 0;
void EnemyBase::OnCollision(GameObject* _other) {
	PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();
	DirectX::XMVECTOR incomingDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat4x4(&position).r[3], DirectX::XMLoadFloat4x4(&(_other->position)).r[3]));
	if(_other->GetTag() == "Bullet") {
		myPhys->rigidBody.AddForce(0.2f, DirectX::XMVectorGetX(incomingDirection), 0.0f, DirectX::XMVectorGetZ(incomingDirection));
		health -= 50.0f;
	}
	if(health <= 0.0f) {
		//Destroy itself
		temp++;
		if(temp > 3) {
			MessageEvents::SendMessage(EVENT_GameWin, EventMessageBase());
			WriteLine("GAME WAS WON");
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(9/*WinCube*/, { 0, 0.75f, 0 }));
		}
		this->Destroy();
	}
}

