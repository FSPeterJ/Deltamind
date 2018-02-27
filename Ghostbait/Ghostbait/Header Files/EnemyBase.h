#pragma once
#include "GameObject.h"
#include "Health.h"

class HexGrid;

class EnemyBase: public GameObject, public Health {
protected:
	enum State {
		IDLE,
		PATROL,
		ATTACK,
		DEATH,
	};
	
	State currState = IDLE;
	float maxSpeed = 2.0f;
	DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	unsigned eventLose;

	bool hurt = false;
	double hurtTimer = 0;
	double hurtDuration = 1;

	bool sentDeathMessage = false;

	void RestartGame() override;

public:
	EnemyBase() { tag = std::string("Enemy");  };
	void Awake(Object* obj) override;
	void Update() override;
	void OnCollision(GameObject* _other);
	void Destroy() override;
	void Disable() override;


	virtual void SetGrid(HexGrid* _grid) {};
	virtual void SetGoal(DirectX::XMFLOAT2 _goal) {};
	virtual void Repath() {};
};
