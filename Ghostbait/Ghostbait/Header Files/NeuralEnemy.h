#pragma once

//#include "NeuralNetwork.h"
#include "EnemyBase.h"
//using namespace GhostMind;
#include "..\Omiracron\Omiracron\Omiracron.h"

struct GameState {
	float distToPlayer, distToCore, playerHealth, myHealth, turretDamages, coreHealth;
	int turretsInRange, enemiesInRange, myType;
};

enum class Action {
	AttackCore,
	AttackPlayer,
	AttackTurret,
	Wander,
};

struct OmiracronInterface {
	static void UpdateState(GameState s) {
		Omiracron::Functions::GiveState(s.distToPlayer, s.distToCore, s.playerHealth, s.coreHealth, s.myHealth, s.turretsInRange, s.enemiesInRange, s.turretDamages, s.myType);
	}
	static void GetNewAction(Action& action) {
		action = static_cast<Action>(Omiracron::Functions::GetAction());
	}
};

class NeuralEnemy {
	GameState currentState;
	Action currentAction;

	void AtkPlayer() {}
	void AtkCore() {}
	void AtkTurret() {}
	void Wander() {}


	void Update() {
		switch(currentAction) {
		default:case Action::AttackCore:
			AtkCore();
			break;
		case Action::AttackPlayer:
			AtkPlayer();
			break;
		case Action::AttackTurret:
			AtkTurret();
			break;
		case Action::Wander:
			Wander();
			break;
		}
	}

	//call this every x frames
	void NeuralUpdate() {
		//currentState.coreHealth = Core.Health;
		//currentState.playerHealth = Player.Health;
		//...
		OmiracronInterface::UpdateState(currentState);
		
		OmiracronInterface::GetNewAction(currentAction);
	}

};
