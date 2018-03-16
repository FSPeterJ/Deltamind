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
	float maxSpeed = 4.0f;
	DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	bool reachedCore = false;
	float attackSpeed = 1;
	float attackDamage = 10;

	unsigned eventLose = 0;
	bool hurt = false;
	double hurtTimer = 0;
	double hurtDuration = 1;
	bool sentDeathMessage = false;

public:
	EnemyBase() { tag = std::string("Enemy");  };
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void Awake(Object* obj) override;
	void Update() override;
	void Subscribe() override;
	void UnSubscribe() override;
	void OnCollision(GameObject* _other) override;


	virtual void SetGrid(HexGrid* _grid) {};
	virtual void SetGoal(DirectX::XMFLOAT2 _goal) {};
	virtual void Repath() {};
};
