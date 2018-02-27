#pragma once
#include "EngineStructure.h"
#include "HexagonalGridLayout.h"
#include "HexGrid.h"
#include <vector>

class SceneManager;
class Spawner;
class EventMessageBase;
class EngineStructure;
class Menu;

enum State {
		GAMESTATE_Paused,
		GAMESTATE_InWave,
		GAMESTATE_BetweenWaves,
		GAMESTATE_Lost
	};

class Game {
	HexGrid hexGrid = HexGrid(50, HexagonalGridLayout::FlatLayout);

	bool run = true;

	struct WaveManager {
		struct Wave {
			struct Spawners {
				int spawnerID = 0;
				std::string enemyName;
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
		std::vector<Spawner*> spawners;
		int enemiesLeftAlive = 0;
		WaveManager waveManager;

		void Reset();
	} gameData;
	
	EngineStructure* engine;
	SceneManager* sceneManager;
	Menu* pauseMenu;

	bool restartNextFrame = false;

	DirectX::XMFLOAT3 corePos = DirectX::XMFLOAT3(0, 0, 0);



	//Event Catchers
	void SpawnerCreatedEvent(EventMessageBase* e);
	void EnemyDiedEvent();
	void PausePressedEvent();
	void SnapRequestEvent(EventMessageBase* e);
	void AddObstacleEvent(EventMessageBase* e);
	void RemoveObstacleEvent(EventMessageBase* e);

	//Personal
	void ChangeState(State newState);
	void ChangeScene(const char* sceneName, DirectX::XMFLOAT3* _corePos = nullptr);
	void StartNextWave();

	//Handle primary function event logic
	void RestartLevel();
	void PauseGame();
	void ResumeGame();
	void Lose();
	void Win();
	void Quit();
	

public:


	Game();

	void Start(EngineStructure* _engine, char* level = "level0");
	void Update();
	void Clean();

	const inline bool Run() { return run; };
	const inline bool IsPaused() const { return gameData.state == GAMESTATE_Paused; };
};
