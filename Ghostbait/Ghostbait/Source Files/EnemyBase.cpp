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
	maxSpeed = (float) ((rand() % 3) + 1);
	target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	isHurting = false;
	hurtTimer = 0;
	hurtDuration = 0.25;
	sentDeathMessage = false;

	pc = GetComponent<PhysicsComponent>();
	rb = &(pc->rigidBody);
	
	eventLose = 0;
	smite = 0;
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
	if(!eventLose) eventLose = MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) { MessageEvents::SendQueueMessage(EVENT_Late, [=] {this->Destroy(); }); });
	if (!smite) smite = MessageEvents::Subscribe(EVENT_Smite, [=](EventMessageBase* e) { this->AdjustHealth(-1000); });

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
	gameObjMutex.lock();
	CalculateResult();
	MessageEvents::SendMessage(EVENT_EnemyDied, EventMessageBase());
	GameObject::Destroy();
	gameObjMutex.unlock();
}

void EnemyBase::TakeDamage(float amount) {
	float damage = AdjustHealth(amount);

	if (!genetics) return;
	genetics->performance.results.damageReceived -= damage;
}

bool EnemyBase::ChangeState(State _s) {
	if (currState == _s) return false;
	
	if (_s == INJURED)
	{
		if(GetComponent<Animator>())
		GetComponent<Animator>()->setState("Taunt");
	}
	else
	{
		if(GetComponent<Animator>())
		GetComponent<Animator>()->setState("Move");
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
	DirectX::XMVECTOR incomingDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat4x4(&transform.GetMatrix()).r[3], DirectX::XMLoadFloat4x4(&(_other->transform.GetMatrix())).r[3]));
	if(_other->GetTag() == "Bullet") {
		pc->rigidBody.AddForce(1.0f, DirectX::XMVectorGetX(incomingDirection), 0.0f, DirectX::XMVectorGetZ(incomingDirection));

		//auto& dam = (((Projectile*) _other)->damage);
		//AdjustHealth(-dam);
		//genetics->performance.results.damageReceived += dam;

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

void EnemyBase::RandomizeStats() {
}
