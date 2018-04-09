#pragma once
#include "GameObject.h"
#include "Health.h"
#include "Core.h"
#include "GhostTime.h"
#include <mutex>

namespace Omiracon {
	namespace Genetics {
		struct Evolvable;
	}
}

using namespace Omiracon::Genetics;

class HexGrid;
class RigidBody;

class EnemyBase: public GameObject, public Health {
protected:
	enum State {
		IDLE,
		PATROL,
		ATTACK,
		DEATH,
	};

	std::mutex enemyMutex;
	Evolvable* genetics = nullptr;
	GhostTime::Moment spawnTime;
	
	State currState = IDLE;
	DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	Core* core = nullptr;

	//float speed = 100.0f;
	//float denfense = 5;
	bool reachedCore = false;
	float maxSpeed = 100.0f;
	float attackSpeed = 1;
	float attackDamage = 5;
	float perceptionRange = 3;
	float timeSinceLastAttack = -1;

	unsigned eventLose = 0;
	unsigned smite = 0;
	bool hurt = false;
	double hurtTimer = 0;
	double hurtDuration = 1;
	bool sentDeathMessage = false;

	RigidBody* rb = 0;

public:
	EnemyBase() { tag = std::string("Enemy");  };
	void Start();
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void Awake(Object* obj) override;
	void Update() override;
	void Subscribe() override;
	void UnSubscribe() override;
	void OnCollision(GameObject* _other) override;
	void DeathEvent() override;
	void RecordAttack();
	void TakeDamage(float amount);
	void Step();
	void CalculateResult();

	Evolvable*& SetTraits() { return genetics; }
	void SetStats();

	virtual void SetGrid(HexGrid* _grid) {};
	virtual void SetCore(Core* _core) { core = _core; };
	virtual void Repath() {};
	virtual void RandomizeStats();

	//virtual void Attack() = 0;
	//virtual void Move() = 0;
	//virtual void TakeDamage(float) = 0;
	//virtual void Death() = 0;
};
