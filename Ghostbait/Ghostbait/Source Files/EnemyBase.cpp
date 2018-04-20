#include "EnemyBase.h"
#include "PhysicsComponent.h"
#include "MessageEvents.h"
#include "Console.h"
#include "Projectile.h"
#include "Material.h"
#include "Animator.h"
#include "Wwise_IDs.h"
#include "Evolvable.h"

void EnemyBase::Awake(Object* obj) {
	currState = IDLE;
	prevState = IDLE;

	genetics = nullptr;
	core = nullptr;
	targetObj = nullptr;
	targetPos = nullptr;

	reachedCore = false;
	isOutofBounds = false;
	isHurting = false;
	isDying = false;
	isChasing = false;
	isRedirecting = false;
	sentDeathMessage = false;

	maxSpeed = (float)((rand() % 3) + 1);
	attackSpeed = 1;
	attackDamage = 5;
	perceptionRange = 3;
	timeSinceLastAttack = -1;

	hurtTimer = 0;
	hurtDuration = 0.25;
	eventLose = 0;
	smite = 0;
	eventObstacleRemove = 0;

	pc = GetComponent<PhysicsComponent>();
	rb = &(pc->rigidBody);

	SetToFullHealth();

	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_RegisterNoisemaker, NewObjectMessage(this));
}

void EnemyBase::Start() {

	if (!genetics) return;
	genetics->performance.Reset();

#undef max
	float domTraits[] = { genetics->traits[STRENGTH] + genetics->traits[POWER] + genetics->traits[ACCURACY] + genetics->traits[LUCK],
						  genetics->traits[INTELLIGENCE] + genetics->traits[WISDOM] + genetics->traits[EVASION] + genetics->traits[DEXTERITY],
						  genetics->traits[DEFENSE] + genetics->traits[ENDURANCE] + genetics->traits[STAMINA] + genetics->traits[RESISTANCE],
						  genetics->traits[SPEED] + genetics->traits[ENERGY] + genetics->traits[COORDINATION] + genetics->traits[BALANCE] };

	auto mx = std::max({domTraits[0], domTraits[1], domTraits[2], domTraits[3]});

	if(mx == domTraits[0]) {
		SwapComponentVarient<Material>("pink");
	} else if(mx == domTraits[1]) {
			SwapComponentVarient<Material>("blue");
	} else if(mx == domTraits[2]) {
		SwapComponentVarient<Material>("green");
	} else if(mx == domTraits[3]) {
		SwapComponentVarient<Material>("yellow");
	}

	SetStats();
	spawnTime = GhostTime::Now();
}

void EnemyBase::Subscribe() {
	if(!eventLose) eventLose = MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) { MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); }); });
	if (!smite) smite = MessageEvents::Subscribe(EVENT_Smite, [=](EventMessageBase* e) { this->AdjustHealth(-1000); });
	if(!eventObstacleRemove) eventObstacleRemove = MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {ValidateTarget(e); });
}

void EnemyBase::UnSubscribe() {
	if(eventLose) {
		MessageEvents::UnSubscribe(EVENT_GameLose, eventLose);
		eventLose = 0;
	}
	if (smite) {
		MessageEvents::UnSubscribe(EVENT_Smite, smite);
		smite = 0;
	}
	if (eventObstacleRemove) {
		MessageEvents::UnSubscribe(EVENT_RemoveObstacle, eventObstacleRemove);
		eventObstacleRemove = 0;
	}
}

void EnemyBase::Enable() {
	EnemyBase::Subscribe();
	GameObject::Enable();
	ChangeState(PATROL);
}

void EnemyBase::Disable() {
	EnemyBase::UnSubscribe();
	GameObject::Disable();
}

void EnemyBase::Destroy() {
	Console::consoleMutex.lock();
	Console::WarningLine << "REGULAR enemy Destory!!";
	Console::consoleMutex.unlock();

	gameObjMutex.lock();
	//isDying = true;
	CalculateResult();
	MessageEvents::SendMessage(EVENT_EnemyDied, EventMessageBase());
	GameObject::Destroy();
	gameObjMutex.unlock();
}

void EnemyBase::SetCore(Core* _core) {
	core = _core;
	targetObj = static_cast<Health*>(_core);
	targetPos = &(_core->transform.matrix._41);
}

bool EnemyBase::ReTarget(GameObject* _obj) {
	Health* tar = dynamic_cast<Health*>(_obj);
	if (targetObj == tar) { return false; }
	if (_obj) {
		targetObj = tar;
		targetPos = &(_obj->transform.matrix._41);
		return true;
	}

	targetObj = core;
	targetPos = &(core->transform.matrix._41);
	return true;
}

void EnemyBase::TakeDamage(float amount) {
	float damage = AdjustHealth(amount);

	if (!genetics) return;
	genetics->performance.results.damageReceived -= damage;
}

void EnemyBase::ValidateTarget(EventMessageBase* e) {
	if (!targetPos) return;
	SnapMessage* s = (SnapMessage*)(e);
	if (s->position->x == targetPos[0] && s->position->y == targetPos[2]) {
		Console::WarningLine << "Target destroyed at: " << s->position->x << ", " << s->position->y;
		ReTarget();
		ChangeState(PATROL);
	}
}

bool EnemyBase::ChangeState(State _s) {
	if (currState == _s || isDying) return false;
	

	switch (_s)
	{
	case EnemyBase::IDLE:
		//Console::WarningLine << "Changing state to IDLE";
		break;
	case EnemyBase::PATROL:
		//Console::WarningLine << "Changing state to PATROL";
		if (GetComponent<Animator>())
			GetComponent<Animator>()->setState("Walk");
		break;
	case EnemyBase::ATTACK:
		//Console::WarningLine << "Changing state to ATTACK";
		isRedirecting = false;
		isChasing = false;
		if (!ValidateAttackTarget()) {
			ReTarget();
			_s = PATROL;
			Console::WarningLine << "Not Near Target!!";
		}
		else {
			if (GetComponent<Animator>())
				GetComponent<Animator>()->setState("Attack");
		}
		break;
	case EnemyBase::INJURED:
		//Console::WarningLine << "Changing state to INJURED";
		if (GetComponent<Animator>())
			GetComponent<Animator>()->setState("Injured");
		break;
	case EnemyBase::DEATH:
		//Console::WarningLine << "Changing state to DEATH";
		isDying = true;
		if (GetComponent<Animator>())
			GetComponent<Animator>()->setState("Death");
		rb->Stop();
		break;
	default:
		break;
	}

	prevState = currState;
	currState = _s;
	return true;
}

void EnemyBase::Update() {
	GameObject::Update();
	gameObjMutex.lock();
	//if(hurt) {
	//	hurtTimer += GhostTime::DeltaTime();
	//	if(hurtTimer >= 0.25f) {
	//		hurt = false;
	//		int id = TypeMap::GetComponentTypeID<Material>();
	//		SetComponent(defaultMat, id);
	//	}
	//}

	//DirectX::XMFLOAT3 vel;
	//DirectX::XMStoreFloat3(&vel, rb->GetVelocity());
	//DirectX::XMFLOAT3 newPoint = {transform.GetPosition().x + vel.x, transform.GetPosition().y, transform.GetPosition().z + vel.z};
	//transform.TurnTowards(newPoint, 1);

	switch (currState)
	{
	case EnemyBase::IDLE:
		Idle();
		break;
	case EnemyBase::PATROL:
		Patrol();
		break;
	case EnemyBase::ATTACK:
		Attack();
		break;
	case EnemyBase::INJURED:
		Injured();
		break;
	case EnemyBase::DEATH:
		Death();
		break;
	default:
		break;
	}

	//DirectX::XMVECTOR directionToGoal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat4x4(&position).r[3]));
	//PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();
	//
	//myPhys->rigidBody.AddForce(0.2f * (float) GhostTime::DeltaTime(), DirectX::XMVectorGetX(directionToGoal), DirectX::XMVectorGetY(directionToGoal), DirectX::XMVectorGetZ(directionToGoal));
	//
	//if(myPhys->rigidBody.GetSpeedSq() > maxSpeed * maxSpeed) {
	//	DirectX::XMVECTOR clampedVelocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(myPhys->rigidBody.GetVelocity()), maxSpeed);
	//	myPhys->rigidBody.SetVelocity(clampedVelocity);
	//}
	gameObjMutex.unlock();
}

#pragma region StateActions

void EnemyBase::Idle() {
	rb->Stop();
}

void EnemyBase::Patrol() {
	DirectX::XMFLOAT3 vel;
	DirectX::XMStoreFloat3(&vel, rb->GetVelocity());
	DirectX::XMFLOAT3 newPoint = { transform.GetPosition().x + vel.x, transform.GetPosition().y, transform.GetPosition().z + vel.z };
	transform.TurnTowards(newPoint, 1);
}

void EnemyBase::Attack() {
	
	//Console::WarningLine << "Attacking";
	//May need to validate if target is next to current?
	if (!targetObj) {
		Console::ErrorLine << "INVALID TARGET!!!";
		ReTarget();
		ChangeState(PATROL);
		return;
	}

	if (!targetObj->IsAlive()) {
		Console::WriteLine << "Target was Destroyed!!";
		ReTarget();
		ChangeState(PATROL);
		timeSinceLastAttack = 0;
		return;
	}

	if (timeSinceLastAttack == -1) {
		targetObj->AdjustHealth(-attackDamage);
		RecordAttack();
		Console::WriteLine << (dynamic_cast<GameObject*>(targetObj))->GetTag().c_str() << ": " << targetObj->PercentHealth();

		timeSinceLastAttack = 0;
		return;
	}

	float dt = (float)GhostTime::DeltaTime();
	timeSinceLastAttack += dt;

	float timeToAttack = 1 / attackSpeed;
	if (timeSinceLastAttack >= timeToAttack) {
		targetObj->AdjustHealth(-attackDamage);
		RecordAttack();
		Console::WriteLine << (dynamic_cast<GameObject*>(targetObj))->GetTag().c_str() << ": " << targetObj->PercentHealth();

		timeSinceLastAttack = 0;
	}
}

void EnemyBase::Injured() {
	if (!isHurting) {
		if (componentVarients.find("Hurt") != componentVarients.end()) {
			int id = TypeMap::GetComponentTypeID<Material>();
			defaultMat = GetComponent<Material>(); //FIXME
			SetComponent(componentVarients["Hurt"], id);
		}
		isHurting = true;
		hurtTimer = 0;
		return;
	}

	hurtTimer += GhostTime::DeltaTime();
	if (hurtTimer >= hurtDuration) {
		//currState = prevState;
		isHurting = false;
		int id = TypeMap::GetComponentTypeID<Material>();
		SetComponent(defaultMat, id);
		ChangeState(prevState);
	}
}

void EnemyBase::Death() {

}

#pragma endregion


#pragma region Genetics

void EnemyBase::RecordAttack() {
	if (!genetics) return;

	genetics->performance.results.damageDelt += attackDamage;
}

void EnemyBase::Step() {
	if (!genetics) return;

	genetics->performance.results.nodesTraversed++;
}

void EnemyBase::CalculateResult() {
	//gameObjMutex.lock();
	if (!genetics) {
		Console::WarningLine << "There's no genetics!!!!";
		return;
	}

	genetics->performance.results.timeLasted = (float)GhostTime::Duration(spawnTime, GhostTime::Now());
	float timeRatio = 1.0f / (genetics->performance.results.timeLasted * 0.001f);
	genetics->performance.results.nodesTraversed *= timeRatio;
	genetics->performance.results.damageDelt *= timeRatio;
	genetics->performance.results.damageReceived *= timeRatio;

	//gameObjMutex.unlock();
}

void EnemyBase::SetStats() {
	if (!genetics) {
		Console::WarningLine << "There's no genetics!!!!";
		return;
	}
	
	gameObjMutex.lock();

	//Play around with the trait effects here
	const float *traitFactors = genetics->traits.GetTraitArray();
	maxSpeed = 3.0f + 6.0f * traitFactors[SPEED] + 4.0f * traitFactors[ENERGY] + 2.0f * traitFactors[COORDINATION] + 1.0f * traitFactors[BALANCE];
	attackDamage = 3.0f + 10.0f * traitFactors[STRENGTH] + 7.0f * traitFactors[POWER] + 4.0f * traitFactors[ACCURACY] + 1.0f * traitFactors[LUCK];
	attackSpeed = 2.0f * traitFactors[INTELLIGENCE] + 1.5f * traitFactors[WISDOM] + 1.0f * traitFactors[EVASION] + 0.5f * traitFactors[DEXTERITY];
	SetMaxHealth(90.0f + 200.0f * traitFactors[DEFENSE] + 150.0f * traitFactors[ENDURANCE] + 100.0f * traitFactors[STAMINA] + 50.0f * traitFactors[RESISTANCE]);
	SetToFullHealth();
	rb->SetTerminalSpeed(maxSpeed);

	gameObjMutex.unlock();
}

#pragma endregion

//overriding Health::DeathEvent()
static int deatheventCount = 0;
void EnemyBase::DeathEvent() {
	//gameObjMutex.lock();
	if (genetics) genetics->performance.died = true;
	sentDeathMessage = true;
	ChangeState(DEATH);
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	deatheventCount++;
	//gameObjMutex.unlock();
}

//Other Overrides
void EnemyBase::OnCollision(GameObject* _other) {
	gameObjMutex.lock();
	//DirectX::XMVECTOR incomingDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat4x4(&transform.GetMatrix()).r[3], DirectX::XMLoadFloat4x4(&(_other->transform.GetMatrix())).r[3]));
	if(_other->GetTag() == "Bullet") {
		//pc->rigidBody.AddForce(1.0f, DirectX::XMVectorGetX(incomingDirection), 0.0f, DirectX::XMVectorGetZ(incomingDirection));

		//auto& dam = (((Projectile*) _other)->damage);
		//AdjustHealth(-dam);
		//genetics->performance.results.damageReceived += dam;

		if (((Projectile*)_other)->isDestroying) {
			gameObjMutex.unlock();
			return;
		}

		TakeDamage(-(((Projectile*) _other)->damage));
		ChangeState(INJURED);

		//if(componentVarients.find("Hurt") != componentVarients.end()) {
		//	if(!isHurting) {
		//		int id = TypeMap::GetComponentTypeID<Material>();
		//		defaultMat = GetComponent<Material>();
		//		SetComponent(componentVarients["Hurt"], id);
		//	}
		//	isHurting = true;
		//	hurtTimer = 0;
		//}
		//if(!IsAlive() && !sentDeathMessage) {
		//	//Destroy itself
		//	//if(temp > 3) {
		//	//	MessageEvents::SendMessage(EVENT_GameWin, EventMessageBase());
		//	//	Console::WriteLine << "GAME WAS WON";
		//	//	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage(9/*WinCube*/, {0, 0.75f, 0}));
		//	//}
		//	//genetics->performance.died = sentDeathMessage = true;
		//	//MessageEvents::SendQueueMessage(EVENT_Late, [=] { Destroy(); });
		//	genetics->performance.died = sentDeathMessage = true;
		//	MessageEvents::SendQueueMessage(EVENT_Late, [=] { Destroy(); });
		//}
	}

	GameObject::OnCollision(_other);
	gameObjMutex.unlock();
}

void EnemyBase::OnTrigger(GameObject* _other) {
	if (isChasing || currState == ATTACK) return;
	if (strcmp(_other->GetTag().c_str(), "Turret") == 0 ) {//|| strcmp(_other->GetTag().c_str(), "Core") == 0) {
		isChasing = ReTarget(_other);
		//ChangeState(ATTACK);
	}
}

void EnemyBase::RandomizeStats() {
}
