#include "TargetEnemy.h"
#include "PhysicsComponent.h"
#include "RandomEngine.h"
#include "MeshManager.h"

void TargetEnemy::SwitchTarget() {
	toEnd = !toEnd;
	if (toEnd) {
		rb->SetVelocity(DirectX::XMLoadFloat3(&s2e));
		transform.LookAt(endPos);
	}
	else {
		rb->SetVelocity(DirectX::XMLoadFloat3(&e2s));
		transform.LookAt(startPos);
	}
}
bool TargetEnemy::IsPastTarget() {
	float actualDist;
	
	if (toEnd) actualDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(transform.GetPosition().x - startPos.x, transform.GetPosition().y - startPos.y, transform.GetPosition().z - startPos.z))));
	else actualDist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(transform.GetPosition().x - endPos.x, transform.GetPosition().y - endPos.y, transform.GetPosition().z - endPos.z))));
	
	return actualDist >= dist;
}

void TargetEnemy::Awake(Object* obj) {
	EnemyBase::Awake(obj);
	toEnd = true;
	speed = 1;

	switch (Omiracon::Random::RandomNumber(0, 3)) {
	case 0:
		SwapComponentVarient<Mesh>("medium");
		SwapComponentVarient<Material>("mediumMat");
		//animator->setState("Walk_Medium");
		enemyType = Medium;
		break;
	case 1:
		SwapComponentVarient<Mesh>("heavy");
		SwapComponentVarient<Material>("heavyMat");
		//animator->setState("Walk_Heavy");
		enemyType = Heavy;
		break;
	case 2:
		SwapComponentVarient<Mesh>("light");
		SwapComponentVarient<Material>("lightMat");
		//animator->setState("Walk_Light");
		enemyType = Light;
		break;
	default:
		SwapComponentVarient<Mesh>("light");
		SwapComponentVarient<Material>("lightMat");
		//animator->setState("Walk_Light");
		enemyType = Light;
		break;
	}

	ChangeState(PATROL);
}
void TargetEnemy::SetRange(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end) {
	startPos = start;
	endPos = end;
	s2e = DirectX::XMFLOAT3(endPos.x - startPos.x, endPos.y - startPos.y, endPos.z - startPos.z);
	e2s = DirectX::XMFLOAT3(startPos.x - endPos.x, startPos.y - endPos.y, startPos.z - endPos.z);
	dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&s2e)));
	rb->SetTerminalSpeed(Omiracon::Random::RandomNumber(0.25f, 2.0f));
	rb->SetVelocity(DirectX::XMLoadFloat3(&s2e));
	MakeInvincible();
}
void TargetEnemy::SetRange(const DirectX::XMFLOAT3& delta) {
	startPos = { transform.GetPosition().x + delta.x, transform.GetPosition().y + delta.y, transform.GetPosition().z + delta.z };
	endPos = { transform.GetPosition().x - delta.x, transform.GetPosition().y - delta.y, transform.GetPosition().z - delta.z };
	s2e = DirectX::XMFLOAT3(endPos.x - startPos.x, endPos.y - startPos.y, endPos.z - startPos.z);
	e2s = DirectX::XMFLOAT3(startPos.x - endPos.x, startPos.y - endPos.y, startPos.z - endPos.z);
	dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&s2e)));
	rb->SetTerminalSpeed(Omiracon::Random::RandomNumber(0.25f, 2.0f));
	rb->SetVelocity(DirectX::XMLoadFloat3(&s2e));
	MakeInvincible();
}

void TargetEnemy::Patrol() {
	if (IsPastTarget()) {
		SwitchTarget();
		timeSinceLastStateChange = 0.0;
	}
	EnemyBase::Patrol();
}

void TargetEnemy::OnCollision(GameObject* _other) {
	gameObjMutex.lock();
	if (_other->GetTag() == "Bullet") {
		ChangeState(INJURED);
	}

	GameObject::OnCollision(_other);
	gameObjMutex.unlock();
}
