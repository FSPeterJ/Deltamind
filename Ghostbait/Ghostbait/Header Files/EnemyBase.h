#pragma once
#include "GameObject.h"
#include "Health.h"

class HexGrid;

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

	bool hurt = false;
	double hurtTimer = 0;
	double hurtDuration = 1;

	void RestartGame() override;

public:
	EnemyBase() { tag = "enemy";  };
	void Update() override;
	void OnCollision(GameObject* _other);

	void CloneData(Object* obj);

	virtual void SetGrid(HexGrid* _grid) {};
	virtual void SetGoal(DirectX::XMFLOAT2 _goal) {};
	virtual void Repath() {};
};
