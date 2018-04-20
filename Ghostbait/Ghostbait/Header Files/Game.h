#pragma once
#include "EngineStructure.h"
#include "HexagonalGridLayout.h"
#include "HexGrid.h"
#include <vector>
#include "Menu.h"
#include "GameData.h"
#include "Evolve.h"

class SceneManager;
class SpawnerObject;
class EventMessageBase;
class EngineStructure;
class Player;
class Core;
class HUD;

using namespace Omiracon::Genetics;


class Game {
	HexGrid hexGrid = HexGrid("Assets/Level0.ghostGrid", HexagonalGridLayout::FlatLayout);
	
	bool run = true;
	
	Evolver evolver;
	GameData gameData = GameData(&evolver);
	std::string currLevelName = "";
	EngineStructure* engine = nullptr;
	SceneManager* sceneManager = nullptr;
	Player* player = nullptr;
	Core* core = nullptr;
	HUD* currHUD;

	Menu pauseMenu;
	Menu mainMenu;
	Menu credits;

	bool paused = false;
	float delayBetweenWaveReady = 3;
	float currentTimeBetweenWaveReady = -1;



	//Event Catchers
	void GameDataRequestEvent(EventMessageBase* e);
	void SpawnerCreatedEvent(EventMessageBase* e);
	void RemoveObstacleEvent(EventMessageBase* e);
	void EnemyDiedEvent();
	void PauseInputEvent();
	void StartEvent(EventMessageBase* e = nullptr);
	void ExitToMainMenu();
	void CoreSetup(EventMessageBase* e);
	void CoreRemoval(EventMessageBase* e);

	//Major Scenes Loaded
	void MainMenuLoaded();
	void TutorialLoaded();
	void Level0Loaded();
	void CreditsLoaded();

	//Personal
	void ChangeState(State newState);
	void ChangeScene(const char* sceneName, std::string levelName = "");
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

	void Start(Player* _player, EngineStructure* _engine, HUD* _hud, char* startScene = "splashScreen", char* xml = "");
	void Update();
	void Clean();

	const inline bool IsRunning() { return run; };
	const inline bool IsPaused() const { return paused; };
};
