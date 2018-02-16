#pragma once
#include "Grid.h"
#include <vector>

class Spawner;
class Scene;
class EventMessageBase;
class EngineStructure;
class Menu;

enum State {
		GAMESTATE_Paused,
		GAMESTATE_InWave,
		GAMESTATE_BetweenWaves
	};

class Game {

	struct WaveManager {
		struct Wave {
			struct Spawners {
				int spawnerID = 0;
				int enemyID = 0;
				int spawnCount = 0;
				float initialDelay = 0;
				float runDelay = 0;
				float timeSinceLastSpawn = 0;
				int enemiesSpawned = 0;
			};
			int reward = 0;
			int enemyCount = 0;
			std::vector<Spawners> spawns;
		};
		std::vector<Wave> waves;
		int currentWave = -1;

	};
	struct GameData {
		State state = GAMESTATE_BetweenWaves;
		State prevState = GAMESTATE_BetweenWaves;
		Scene* currentScene = nullptr;
		std::vector<Spawner*> spawners;
		int enemiesLeftAlive = 0;
		WaveManager waveManager;

		void Reset();
	} gameData;
	
	EngineStructure* engine;
	Menu* pauseMenu;

	//Personal
	void ChangeState(State newState);
	void LoadLevel(char* fileName);
	void StartNextWave();

	void PauseGame();
	void ResumeGame();
	void Lose();
	void Win();
	
	//Delegate Subscription Calls
	void SpawnerCreatedEvent(EventMessageBase* e);
	void EnemyDiedEvent();
	void StartPressedEvent();
	void PausePressedEvent();
	void RestartGameEvent();

public:
	Grid mygrid = Grid(20, 20, 1, 1, 0, 0);
	
	Game();

	void Start(EngineStructure* _engine, char* level = "Game Files/level0.xml");
	void Update();
	void Clean();

	const inline bool IsPaused() const { return gameData.state == GAMESTATE_Paused; };
};
