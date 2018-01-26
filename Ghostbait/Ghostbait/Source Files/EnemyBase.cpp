#include "EnemyBase.h"
#include "GhostTime.h"
#include "PhysicsComponent.h"

void EnemyBase::Update() {
	DirectX::XMVECTOR directionToGoal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat4x4(&position).r[3]));
	PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();
	
	myPhys->rigidBody.AddForce(10.0f * (float)GhostTime::SmoothDeltaTime(), DirectX::XMVectorGetX(directionToGoal), DirectX::XMVectorGetY(directionToGoal), DirectX::XMVectorGetZ(directionToGoal));

	if (myPhys->rigidBody.GetSpeedSq() > maxSpeed * maxSpeed) {
		DirectX::XMVECTOR clampedVelocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(myPhys->rigidBody.GetVelocity()), maxSpeed);
		myPhys->rigidBody.SetVelocity(clampedVelocity);
	}
}

void EnemyBase::OnCollision(PhysicsComponent* _other) {
	PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();
	DirectX::XMVECTOR incomingDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat4x4(&position).r[3], DirectX::XMLoadFloat4x4(&(_other->parentObject->position)).r[3]));
	//if bullet
	//myPhys->rigidBody.AddForce(5.0f, DirectX::XMVectorGetX(incomingDirection), DirectX::XMVectorGetY(incomingDirection), DirectX::XMVectorGetZ(incomingDirection));
	//health -= 50.0f;

	if (health <= 0.0f) {
		//Destroy itself
	}
}

