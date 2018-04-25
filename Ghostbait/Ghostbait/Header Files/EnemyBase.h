#pragma once
#include "GameObject.h"
#include "Health.h"
#include "Core.h"
#include "GhostTime.h"
#include <mutex>

#define DEFAULT_WALK_RATIO 1.0f / 2.2f
#define LIGHT_WALK_RATIO 1.0f / 2.9f
#define MEDIUM_WALK_RATIO 1.0f / 2.0f
#define HEAVY_WALK_RATIO 1.0f / 2.0f


namespace Omiracon {
	namespace Genetics {
		struct Evolvable;
	}
}

using namespace Omiracon::Genetics;

class HexGrid;
class RigidBody;
struct PhysicsComponent;
class Animator;

class EnemyBase: public GameObject, public Health {
protected:
	enum State {
		IDLE,
		PATROL,
		ATTACK,
		INJURED,
		DEATH,
	};
	enum EnemyType {
		Light,
		Medium,
		Heavy,
		Default,
	};

	std::mutex enemyMutex;
	State currState = IDLE, prevState = IDLE;
	
	GhostTime::Moment spawnTime;
	Evolvable* genetics = nullptr;
	EnemyType enemyType = Default;

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
	bool isRedirecting = false;
	bool sentDeathMessage = false;

	float maxSpeed = 3.0f;
	float attackSpeed = 1;
	float attackDamage = 5;
	float timeSinceLastAttack = -1;
	int perceptionRange = 3;
	int attackRange = 1;

	DirectX::XMFLOAT3 prevVelocity = {0.0f, 0.0f, 0.0f};
	double hurtTimer = 0;
	double hurtDuration = 1;
	double deathTimer = 0;
	double deathDuration = 1.23;
	double deathDuration2 = 2.46;
	unsigned eventLose = 0;
	unsigned smite = 0;
	unsigned eventObstacleRemove = 0;

	PhysicsComponent* pc = 0;
	Animator* animator = 0;
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
	void ValidateTarget(EventMessageBase* e);

	//Returns True if the current state was changed
	bool ChangeState(State _s);

	Evolvable*& SetTraits() { return genetics; }
	void SetStats();
	void RecordAttack();
	void Step();
	void CalculateResult();

	virtual void SetGrid(HexGrid* _grid) {};
	virtual void SetCore(Core* _core);
	virtual void Repath() {};
	virtual void RandomizeStats();
	virtual bool ReTarget(GameObject* _obj = nullptr);
	virtual bool ValidateAttackTarget() { return true; };

	virtual void Idle();
	virtual void Patrol();
	virtual void Attack();
	virtual void Injured();
	virtual void Death();

	//virtual void TakeDamage(float) = 0;
	//virtual void Death() = 0;
};
