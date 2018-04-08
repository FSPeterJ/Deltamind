#include "EnemyBase.h"
#include "PhysicsComponent.h"
#include "MessageEvents.h"
#include "Console.h"
#include "Projectile.h"
#include "Material.h"

#include "Evolvable.h"

void EnemyBase::Awake(Object* obj) {
	currState = IDLE;
	maxSpeed = (float) ((rand() % 3) + 1);
	target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	hurt = false;
	hurtTimer = 0;
	hurtDuration = 1;
	sentDeathMessage = false;
	rb = 0;

	
	eventLose = 0;
	smite = 0;
	SetToFullHealth();
	GameObject::Awake(obj);
}

void EnemyBase::Start() {
	genetics->performance.Reset();

#undef max
	auto mx = std::max({genetics->traits[STRENGTH],genetics->traits[INTELLIGENCE],genetics->traits[DEFENSE],genetics->traits[SPEED]});

	if(mx == genetics->traits[STRENGTH]) {
		SwapComponentVarient<Material>("pink");
	} else if(mx == genetics->traits[INTELLIGENCE]) {
			SwapComponentVarient<Material>("blue");
	} else if(mx == genetics->traits[DEFENSE]) {
		SwapComponentVarient<Material>("green");
	} else if(mx == genetics->traits[SPEED]) {
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
}
void EnemyBase::Disable() {
	EnemyBase::UnSubscribe();
	GameObject::Disable();
}
void EnemyBase::Destroy() {
	CalculateResult();
	MessageEvents::SendMessage(EVENT_EnemyDied, EventMessageBase());
	GameObject::Destroy();
}

void EnemyBase::RecordAttack() {
	genetics->performance.results.damageDelt += attackDamage;
}

void EnemyBase::Step() {
	genetics->performance.results.nodesTraversed++;
}

void EnemyBase::TakeDamage(float amount) {
	genetics->performance.results.damageReceived += -AdjustHealth(amount);
}

void EnemyBase::CalculateResult() {
	genetics->performance.results.timeLasted = (float)GhostTime::Duration(spawnTime, GhostTime::Now());
	float timeRatio = 1.0f / (genetics->performance.results.timeLasted * 0.001f);
	genetics->performance.results.nodesTraversed *= timeRatio;
	genetics->performance.results.damageDelt *= timeRatio;
	genetics->performance.results.damageReceived *= timeRatio;
}

void EnemyBase::SetStats() {
	if (!genetics) return;
	
	//Play around with the trait effects here
	const float *traitFactors = genetics->traits.GetTraitArray();
	maxSpeed = 4.0f + 6.0f * traitFactors[SPEED] + 4.0f * traitFactors[ENERGY] + 2.0f * traitFactors[COORDINATION] + 1.0f * traitFactors[BALANCE];
	attackDamage = 3.0f + 10.0f * traitFactors[STRENGTH] + 7.0f * traitFactors[POWER] + 4.0f * traitFactors[ACCURACY] + 1.0f * traitFactors[LUCK];
	attackSpeed = 2.0f * traitFactors[SPEED] + 1.5f * traitFactors[ENERGY] + 1.0f * traitFactors[COORDINATION] + 0.5f * traitFactors[BALANCE];
	SetMaxHealth(90.0f + 200.0f * traitFactors[DEFENSE] + 150.0f * traitFactors[ENDURANCE] + 100.0f * traitFactors[STAMINA] + 50.0f * traitFactors[RESISTANCE]);
	SetToFullHealth();
}

void EnemyBase::Update() {
	if(hurt) {
		hurtTimer += GhostTime::DeltaTime();
		if(hurtTimer >= 0.25f) {
			hurt = false;
			int id = TypeMap::GetComponentTypeID<Material>();
			SetComponent(defaultMat, id);
		}
	}

	DirectX::XMFLOAT3 vel;
	DirectX::XMStoreFloat3(&vel, rb->GetVelocity());
	DirectX::XMFLOAT3 newPoint = {transform.GetPosition().x + vel.x, transform.GetPosition().y, transform.GetPosition().z + vel.z};
	transform.TurnTowards(newPoint, 1);

	GameObject::Update();
	//DirectX::XMVECTOR directionToGoal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat4x4(&position).r[3]));
	//PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();
	//
	//myPhys->rigidBody.AddForce(0.2f * (float) GhostTime::DeltaTime(), DirectX::XMVectorGetX(directionToGoal), DirectX::XMVectorGetY(directionToGoal), DirectX::XMVectorGetZ(directionToGoal));
	//
	//if(myPhys->rigidBody.GetSpeedSq() > maxSpeed * maxSpeed) {
	//	DirectX::XMVECTOR clampedVelocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(myPhys->rigidBody.GetVelocity()), maxSpeed);
	//	myPhys->rigidBody.SetVelocity(clampedVelocity);
	//}
}

void EnemyBase::DeathEvent() {
	genetics->performance.died = sentDeathMessage = true;
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
}

//Other Overrides
void EnemyBase::OnCollision(GameObject* _other) {
	PhysicsComponent* myPhys = GetComponent<PhysicsComponent>();//component doesn't change?? this needs cached. set on awake or start.
	DirectX::XMVECTOR incomingDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat4x4(&transform.GetMatrix()).r[3], DirectX::XMLoadFloat4x4(&(_other->transform.GetMatrix())).r[3]));
	if(_other->GetTag() == "Bullet") {
		myPhys->rigidBody.AddForce(0.2f, DirectX::XMVectorGetX(incomingDirection), 0.0f, DirectX::XMVectorGetZ(incomingDirection));

		//auto& dam = (((Projectile*) _other)->damage);
		//AdjustHealth(-dam);
		//genetics->performance.results.damageReceived += dam;

		TakeDamage(-(((Projectile*) _other)->damage));

		if(componentVarients.find("Hurt") != componentVarients.end()) {
			if(!hurt) {
				int id = TypeMap::GetComponentTypeID<Material>();
				defaultMat = GetComponent<Material>();
				SetComponent(componentVarients["Hurt"], id);
			}
			hurt = true;
			hurtTimer = 0;
		}
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
}
void EnemyBase::RandomizeStats() {
}
