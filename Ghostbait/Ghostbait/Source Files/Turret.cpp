
#include "Turret.h"

#include <DirectXMath.h>
#include "DebugRenderer.h"
#include "Animator.h"


Turret::Turret() {
	tag = std::string("Turret");
	targetDistance = 9999999;
}

void Turret::Update() {
	if(target) {
		DirectX::XMFLOAT4X4* temp = GetComponent<Animator>()->getJointByName("RocketLauncher_DeployedSetup:Pitch");
		DebugRenderer::AddLine((DirectX::XMFLOAT3)temp->m[3], (DirectX::XMFLOAT3)target->position.m[3], DirectX::XMFLOAT3(1.0f, 0.6f, 0.0f));

		targetDistance = CalculateDistance(target);
		if(targetDistance > 5) {
			target = nullptr;
			targetDistance = 99999;

		}
	}

}

float Turret::CalculateDistance(GameObject* obj) {
	float length;
	using namespace DirectX;
	DirectX::XMFLOAT4X4* temp = GetComponent<Animator>()->getJointByName("RocketLauncher_DeployedSetup:Pitch");

	XMVECTOR pos = XMLoadFloat3(&(XMFLOAT3)temp->m[3]);
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

