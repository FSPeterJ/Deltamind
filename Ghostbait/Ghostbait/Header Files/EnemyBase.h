#pragma once
#include "GameObject.h"
#include "Health.h"

class EnemyBase: public GameObject, public Health {
	enum State {
		IDLE,
		PATROL,
		ATTACK,
		DEATH,
	};

	State currState = IDLE;
	float maxSpeed = 1.0f;
	DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	void RestartGame() override;

public:
	EnemyBase() { tag = "enemy";  };
	void Update() override;
	void OnCollision(GameObject* _other);
};
