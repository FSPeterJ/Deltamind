#pragma once
#include "EngineStructure.h"
#include "HexagonalGridLayout.h"
#include "HexGrid.h"
#include <vector>
#include "Menu.h"

#include "Evolve.h"
using namespace Omiracon::Genetics;

class SceneManager;
class SpawnerObject;
class EventMessageBase;
class EngineStructure;
class Player;
class Core;

enum State {
	GAMESTATE_InWave,
	GAMESTATE_BetweenWaves,
	GAMESTATE_GameOver,
	GAMESTATE_SplashScreen,
	GAMESTATE_MainMenu,
};

class Game {
	HexGrid hexGrid = HexGrid("Assets/Level0.ghostGrid", HexagonalGridLayout::FlatLayout);

	Evolver evolver = Evolver(20);
	bool run = true;

	struct WaveManager {
		struct Wave {
			struct SpawnerData {
				int spawnerID = 0;
				std::string enemyName;
				int spawnCount = 0;
				float initialDelay = 0;
				float runDelay = 0;
				float timeSinceLastSpawn = 0;
				int enemiesSpawned = 0;
				inline void UpdateTimer(const float& dt) { timeSinceLastSpawn += dt; }
				inline void RestartTimer() { timeSinceLastSpawn = 0; }
				inline const int EnemiesToSpawn() const { return spawnCount - enemiesSpawned; }
				inline const bool ReadyToSpawn(const float& nextSpawnTime) const { return timeSinceLastSpawn >= nextSpawnTime; }
			};
			int reward = 0;
			int enemyCount = 0;
			std::vector<SpawnerData> spawns;
		};
	private:
		std::vector<Wave> waves;
		int currentWave = -1;
		std::vector<SpawnerObject*> spawnerObjects;
		int enemiesLeftAlive = 0;
		Game& game;
	public:
		void MoveToNextWave();
		inline const bool NextWaveExists() const { return (currentWave + 1 < (int) waves.size()); }
		inline const int GetCurrentWaveReward() const { return waves[currentWave].reward; }
		inline void AddWave(const WaveManager::Wave& newWave) { waves.push_back(newWave); }
		inline const int GetWaveCount() const { return (int) waves.size(); }
		inline const int GetAliveEnemyCount() const { return enemiesLeftAlive; }
		inline void EnemyKilled(const int& enemyCount = 1) { enemiesLeftAlive -= enemyCount; }

		inline const int GetSpawnObjectCount() const { return (int) spawnerObjects.size(); }
		inline void AddSpawnerObject(SpawnerObject* spawner) { spawnerObjects.push_back(spawner); }
		inline const SpawnerObject* GetSpawnObject(const int& index) { return spawnerObjects[index]; }
		inline const int GetSpawnCount() const { return (int) waves[currentWave].spawns.size(); }
		inline WaveManager::Wave::SpawnerData* GetSpawnerData(const int& index) { return &waves[currentWave].spawns[index]; }
		void SpawnEnemy(WaveManager::Wave::SpawnerData* spawnerData, const int& spawnerObjectIndex, HexGrid* grid, Core* _core);

		void Reset();

		WaveManager(Game& x) : game(x) {}
	};

	struct SplashScreenManager {
		struct LogoData {
			std::string fileName;
			float spawnTime;
		};

	private:
		std::string nextScene = "";
		float timeInScene = 0;
		float sceneTimeLimit = -1;
		int currentLogoIndex = -1;
		GameObject* currentLogo = nullptr;
		std::vector<LogoData> logos;
	public:
		inline void UpdateTimeInScene(const float dt) { timeInScene += dt; }
		inline void SetSceneTimeLimit(const float& timeLimit) { sceneTimeLimit = timeLimit; }
		inline void SetNextScene(const std::string& sceneName) { nextScene = sceneName; }
		inline const std::string& GetNextScene() const { return nextScene; }
		inline const bool TimeLimitReached() const { return timeInScene >= sceneTimeLimit; }
		inline const bool HasTimeLimit() const { return sceneTimeLimit > 0; }
		inline const bool TimeLimitOnLastObject() const { return sceneTimeLimit == -2; }
		inline const bool TimeToSpawnNextLogo() const { return ((currentLogoIndex <= (int) logos.size() && currentLogoIndex >= 0) ? timeInScene >= logos[currentLogoIndex].spawnTime : -1); }

		inline void AddLogo(const SplashScreenManager::LogoData& newLogo) { logos.push_back(newLogo); }
		inline GameObject* GetCurrentLogoObject() const { return currentLogo; }
		inline void SetCurrentLogoObject(GameObject* newLogo) { currentLogo = newLogo; }
		inline const LogoData& GetCurrentLogoData() const { return logos[currentLogoIndex]; }
		inline const int GetCurrentLogoIndex() const { return currentLogoIndex; }
		inline const int GetLogoCount() const { return (int) logos.size(); }
		inline void ClearLogos() { while(logos.size()) logos.erase(logos.begin()); }
		inline const bool NextLogoExists() { return (currentLogoIndex + 1 < (int) logos.size()); }
		inline void MoveToNextLogo() { ++currentLogoIndex; }
		inline void UpdateCurrentLogoSpawnTime() { logos[currentLogoIndex].spawnTime += timeInScene; }

		void Reset();
	};

	class GameData {
		State state = GAMESTATE_BetweenWaves;
		State prevState = GAMESTATE_BetweenWaves;
	public:
		unsigned gears = 0;
		unsigned turretsSpawned = 0;
		unsigned maxTurrets = 100;
		WaveManager waveManager;
		SplashScreenManager ssManager;

		inline const State& GetState() const { return state; }
		inline const State& GetPrevState() const { return prevState; }
		inline void SetPrevStateHard(const State& newState) { prevState = newState; }
		inline void SetStateHard(const State& newState) { state = newState; }
		void UpdateState(const State& newState);

		void Reset();
		GameData(Game& x) : waveManager(WaveManager(x)) {}

	} gameData = GameData(*this);

	EngineStructure* engine;
	SceneManager* sceneManager;
	Player* player;

	Menu pauseMenu;
	Menu mainMenu;

	bool paused = false;

	Core* core = nullptr;

	//Event Catchers
	void SpawnerCreatedEvent(EventMessageBase* e);
	void RemoveObstacleEvent(EventMessageBase* e);
	void EnemyDiedEvent();
	void PauseInputEvent();
	void StartEvent();
	void ExitToMenu();

	//Personal
	void ChangeState(State newState);
	void ChangeScene(const char* sceneName);
	void StartNextWave();

	//Handle primary function event logic
	void BecameGod();
	void RestartLevel();
	void PauseGame();
	void ResumeGame();
	void Lose();
	void Win();
	void Quit();

public:
	Game();

	void Start(Player* _player, EngineStructure* _engine, char* level = "splashScreen");
	void Update();
	void Clean();

	const inline bool Run() { return run; }; //this should be renamed to IsRunning or something similar
	const inline bool IsPaused() const { return paused; };
};
