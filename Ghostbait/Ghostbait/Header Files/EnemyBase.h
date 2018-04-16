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
struct PhysicsComponent;

class EnemyBase: public GameObject, public Health {
protected:
	enum State {
		IDLE,
		PATROL,
		ATTACK,
		INJURED,
		DEATH,
	};

	std::mutex enemyMutex;
	State currState = IDLE, prevState = IDLE;
	
	GhostTime::Moment spawnTime;
	Evolvable* genetics = nullptr;

	Core* core = nullptr;
	Health* targetObj = nullptr;
	float* targetPos = nullptr;

	//float speed = 100.0f;
	//float denfense = 5;
	bool reachedCore = false;
	bool isOutofBounds = false;
	bool isHurting = false;
	bool isDying = false;
	bool isChasing = false;
	bool sentDeathMessage = false;

	float maxSpeed = 100.0f;
	float attackSpeed = 1;
	float attackDamage = 5;
	float perceptionRange = 3;
	float timeSinceLastAttack = -1;

	double hurtTimer = 0;
	double hurtDuration = 1;
	unsigned eventLose = 0;
	unsigned smite = 0;

	PhysicsComponent* pc = 0;
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
	void OnTrigger(GameObject* _other) override;
	void DeathEvent() override;
	void TakeDamage(float amount);

	//Returns True if the current state was changed
	bool ChangeState(State _s);

	Evolvable*& SetTraits() { return genetics; }
	void SetStats();
	void RecordAttack();
	void Step();
	void CalculateResult();

	virtual void SetGrid(HexGrid* _grid) {};
	virtual void SetCore(Core* _core) { targetObj = core = _core; };
	virtual void Repath() {};
	virtual void RandomizeStats();
	virtual bool ReTarget(GameObject* _obj = nullptr);

	virtual void Idle();
	virtual void Patrol();
	virtual void Attack();
	virtual void Injured();
	virtual void Death();

	//virtual void TakeDamage(float) = 0;
	//virtual void Death() = 0;
};
