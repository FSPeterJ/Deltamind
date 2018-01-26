#pragma once
#include "GameObject.h"

struct PhysicsComponent;

class EnemyBase : public GameObject {
	enum State {
		IDLE,
		PATROL,
		ATTACK,
		DEATH,
	};

	State currState = IDLE;
	float health = 100.0f;
	float maxSpeed = 4.0f;
	DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	

	void Update() override;
	void OnCollision(PhysicsComponent* _other);
};
