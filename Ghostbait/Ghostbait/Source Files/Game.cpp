#include <time.h>
#include "Game.h"
#include "Spawner.h"
#include "MessageEvents.h"
#include "GhostTime.h"
#include "EngineStructure.h"
#include "Menu.h"
#include "../Dependencies/XML_Library/irrXML.h"
#include "AStarEnemy.h"
#include "SceneManager.h"
#include "PathPlanner.h"
#include "ObjectFactory.h"
#include "Player.h"

void Game::GameData::Reset() {
	state = GAMESTATE_BetweenWaves;
	prevState = GAMESTATE_BetweenWaves;
	while (spawners.size() > 0) {
		spawners.erase(spawners.begin());
	}
	enemiesLeftAlive = 10000;
	while (waveManager.waves.size() > 0) {
		waveManager.waves.erase(waveManager.waves.begin());
	}
	waveManager.currentWave = -1;
	nextScene = "";
	timeInScene = 0;
	sceneTimeLimit = -1;
	currentLogoIndex = -1;
	currentLogo = nullptr;
	while (logos.size()) {
		logos.erase(logos.begin());
	}
}

Game::Game() {
	pauseMenu = new Menu(MENU_Pause);
	mainMenu = new Menu(MENU_Main);
	MessageEvents::Subscribe(EVENT_SpawnerCreated, [=](EventMessageBase* e) {this->SpawnerCreatedEvent(e); });
	MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) {this->EnemyDiedEvent(); });
	MessageEvents::Subscribe(EVENT_Start, [=](EventMessageBase* e) {this->StartEvent(); });
	MessageEvents::Subscribe(EVENT_PauseInputDetected, [=](EventMessageBase* e) {this->PauseInputEvent(); });
	MessageEvents::Subscribe(EVENT_GamePause, [=](EventMessageBase* e) {this->PauseGame(); });
	MessageEvents::Subscribe(EVENT_GameUnPause, [=](EventMessageBase* e) {this->ResumeGame(); });
	MessageEvents::Subscribe(EVENT_GameRestart, [=](EventMessageBase* e) {this->RestartLevel(); });
	MessageEvents::Subscribe(EVENT_SnapRequest, [=](EventMessageBase* e) {this->SnapRequestEvent(e); });
	MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) {this->AddObstacleEvent(e); });
	MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->RemoveObstacleEvent(e); });
	MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) {this->Lose(); });
	MessageEvents::Subscribe(EVENT_GameQuit, [=](EventMessageBase* e) {this->Quit(); });
	MessageEvents::Subscribe(EVENT_GameExit, [=](EventMessageBase* e) {this->ExitToMenu(); });
	PathPlanner::SetGrid(&hexGrid);
}

//Catch Events
void Game::PauseInputEvent() {
	if (gameData.state == GAMESTATE_SplashScreen || gameData.state == GAMESTATE_MainMenu) return;
	if (paused) {
		ResumeGame();
		MessageEvents::SendMessage(EVENT_GameUnPause, EventMessageBase());
	}
	else {
		PauseGame();
		MessageEvents::SendMessage(EVENT_GamePause, EventMessageBase());
	}
}
void Game::SpawnerCreatedEvent(EventMessageBase* e) {
	Spawner* spawner = dynamic_cast<Spawner*>(((SpawnerCreatedMessage*)e)->RetrieveObject());
	if (spawner)
		gameData.spawners.push_back(spawner);
	else
		Console::ErrorLine << "A non-spawner Object was send to Game.h in the SpawnerCreated Message!";
}
void Game::EnemyDiedEvent() {
	gameData.enemiesLeftAlive--;
	if (gameData.enemiesLeftAlive <= 0) {
		ChangeState(GAMESTATE_BetweenWaves);
	}
}
void Game::SnapRequestEvent(EventMessageBase* e) {
	SnapMessage* message = (SnapMessage*)e;
	DirectX::XMFLOAT2 givenPos = *message->position;
	(*message->success) = hexGrid.Snap(givenPos, *message->position);
}
void Game::AddObstacleEvent(EventMessageBase* e) {
	SnapMessage* message = (SnapMessage*)e;
	*message->success = false;
	if (!hexGrid.IsBlocked(*message->position)) {
		*message->success = hexGrid.AddObstacle(*message->position);
	}
}
void Game::RemoveObstacleEvent(EventMessageBase* e) {
	SnapMessage* message = (SnapMessage*)e;
	*message->success = false;
	if (hexGrid.IsBlocked(*message->position)) {
		(*message->success) = hexGrid.RemoveObstacle(*message->position);
	}
}
void Game::StartEvent() {
	switch (gameData.state) {
		case GAMESTATE_SplashScreen:
			{
				char* sceneName = new char[gameData.nextScene.length() + 1];
				memcpy(sceneName, gameData.nextScene.c_str(), gameData.nextScene.length() + 1);
				ChangeScene(sceneName);
				delete sceneName;
			}
			break;
		case GAMESTATE_BetweenWaves:
			StartNextWave();
			break;
		case GAMESTATE_MainMenu:
			ChangeScene("level0");
			break;
	}
}

//Helpers
void Game::ChangeState(State newState) {
	if (gameData.state != newState) {
		gameData.prevState = gameData.state;
		gameData.state = newState;


		switch (newState) {
			case GAMESTATE_BetweenWaves:
				{
					//if upcoming wave doesnt exist...
					int nextWave = gameData.waveManager.currentWave + 1;
					if (nextWave >= gameData.waveManager.waves.size()) {
						Win();
					}
					//if upcoming wave does exist
					else if (gameData.prevState == GAMESTATE_InWave) {
						//Spawn start cube
						MenuCube* startCube;
						unsigned ID = ObjectFactory::CreatePrefab(&std::string("Assets/StartCube.ghost"));
						MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuCube>(ID, { 0, 1.5f, 0.0f }, &startCube));
						DirectX::XMFLOAT4X4 newPos;
						DirectX::XMStoreFloat4x4(&newPos, DirectX::XMLoadFloat4x4(&startCube->transform.GetMatrix()) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
						startCube->transform.SetMatrix(newPos);
						startCube->Enable();
					}
				}
				break;
			case GAMESTATE_InWave:
				{

				}
				break;
			case GAMESTATE_GameOver:
				{
					//gameData.Reset();
				}
				break;
			case GAMESTATE_SplashScreen:
				{

				}
				break;
			case GAMESTATE_MainMenu:
				break;
		}
	}
}
void Game::ChangeScene(const char* sceneName) {
	//Delete all current scene Items
	MessageEvents::SendMessage(EVENT_DeleteAllGameObjects, EventMessageBase());

	//Reset Game/Wave data
	gameData.Reset();

	//Load scene assets
	sceneManager->LoadScene(sceneName, &corePos);

	//TODO: TEMPORARY main menu code--------
	if (!strcmp(sceneName, "mainMenu")) {
		DirectX::XMFLOAT4X4 menuPos = DirectX::XMFLOAT4X4(1, 0, 0, 0, 
															0, 1, 0, 0, 
															0, 0, 1, 0, 
															0, 1.7f, 2, 1);
		gameData.state = GAMESTATE_MainMenu;
		mainMenu->Show(&menuPos);
		player->leftController->SetControllerState(CSTATE_MenuController);
		player->rightController->SetControllerState(CSTATE_None);
		player->transform.MoveToOrigin(player->playerHeight);
	}
	//--------------------------------------

	//If it has level/wave data, load it
	if (sceneManager->GetCurrentScene().levelFiles.size() > 0) {
		irr::io::IrrXMLReader *xmlReader = irr::io::createIrrXMLReader(sceneManager->GetCurrentScene().levelFiles[0].c_str());

		WaveManager::Wave* newWave = nullptr;
		while (xmlReader->read()) {
			if (xmlReader->getNodeType() == irr::io::EXN_ELEMENT) {
				if (!strcmp("Level", xmlReader->getNodeName())) {
					gameData.state = GAMESTATE_BetweenWaves;
					gameData.prevState = GAMESTATE_BetweenWaves;
					player->leftController->SetControllerState(CSTATE_Inventory);
					player->rightController->SetControllerState(CSTATE_Inventory);
				}
				else if (!strcmp("MenuScene", xmlReader->getNodeName())) {
					gameData.sceneTimeLimit = xmlReader->getAttributeValueAsFloat("sceneTimeLimit");
					gameData.nextScene = xmlReader->getAttributeValue("nextScene");
					gameData.state = GAMESTATE_SplashScreen;
					gameData.prevState = GAMESTATE_SplashScreen;
					player->leftController->SetControllerState(CSTATE_ModelOnly);
					player->rightController->SetControllerState(CSTATE_ModelOnly);
				}
				else if (!strcmp("Wave", xmlReader->getNodeName())) {
					if (newWave) {
						gameData.waveManager.waves.push_back(*newWave);
						delete newWave;
					}
					newWave = new WaveManager::Wave();
					(*newWave).reward = xmlReader->getAttributeValueAsInt("reward");
				}
				else if (!strcmp("Spawner", xmlReader->getNodeName())) {
					WaveManager::Wave::Spawners newSpawner;
					newSpawner.spawnerID = xmlReader->getAttributeValueAsInt("spawnerID");
					newSpawner.enemyName = xmlReader->getAttributeValueSafe("enemyName");
					newSpawner.spawnCount = xmlReader->getAttributeValueAsInt("spawnCount");
					newSpawner.initialDelay = xmlReader->getAttributeValueAsFloat("initialDelay");
					newSpawner.runDelay = xmlReader->getAttributeValueAsFloat("runDelay");
					(*newWave).spawns.push_back(newSpawner);
					(*newWave).enemyCount += newSpawner.spawnCount;
				}
				else if (!strcmp("Logo", xmlReader->getNodeName())) {
					Logo logo;
					logo.fileName = xmlReader->getAttributeValue("ghostFile");
					logo.duration = xmlReader->getAttributeValueAsFloat("duration");
					gameData.logos.push_back(logo);
				}
			}
		}
		if (newWave) {
			gameData.waveManager.waves.push_back(*newWave);
			delete newWave;
		}
		delete xmlReader;
	}
}
void Game::StartNextWave() {
	int nextWave = gameData.waveManager.currentWave + 1;
	if (nextWave >= gameData.waveManager.waves.size()) {
		Console::ErrorLine << "You tried to progress to a wave that doesnt exist!";
	}
	else {
		gameData.enemiesLeftAlive = gameData.waveManager.waves[++gameData.waveManager.currentWave].enemyCount;
		ChangeState(GAMESTATE_InWave);
	}
}

//Handle primary function event logic
void Game::RestartLevel() {
	//Reset currentScene pointer
	std::string name = sceneManager->GetNameFromScene(sceneManager->ResetCurrentScene());

	//Reinstantiate starting wave values and current scene
	ChangeScene(name.c_str());
}
void Game::ResumeGame() {
	//Logic to run when game first gets unPaused
	if (!paused) return;
	paused = false;
	player->leftController->SetControllerStateToPrevious();
	player->rightController->SetControllerStateToPrevious();
}
void Game::PauseGame() {
	//Logic to run when game first gets paused
	if (paused) return;
	paused = true;
	player->leftController->SetControllerState(CSTATE_MenuController);
	player->rightController->SetControllerState(CSTATE_ModelOnly);
}
void Game::Lose() {
	//Logic to run when the player loses
	MenuCube* loseCube;
	unsigned ID = ObjectFactory::CreatePrefab(&std::string("Assets/LoseCube.ghost"));
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuCube>(ID, { 0, 0.75f, 0 }, &loseCube));
	DirectX::XMFLOAT4X4 newPos;
	DirectX::XMStoreFloat4x4(&newPos, DirectX::XMLoadFloat4x4(&loseCube->transform.GetMatrix()) * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));
	loseCube->transform.SetMatrix(newPos);
	loseCube->Enable();

	ChangeState(GAMESTATE_GameOver);
}
void Game::Win() {
	//Logic to run when the player wins
	MessageEvents::SendMessage(EVENT_GameWin, EventMessageBase());
	Console::WriteLine << "GAME WAS WON";
	MenuCube* winCube;
	unsigned ID = ObjectFactory::CreatePrefab(&std::string("Assets/WinCube.ghost"));
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuCube>(ID, { 0, 0.75f, 0 }, &winCube));
	winCube->Enable();
}
void Game::Quit() {
	run = false;
}
void Game::ExitToMenu() {
	ChangeScene("mainMenu");
}

//Main loop elements
void Game::Start(Player* _player, EngineStructure* _engine, char* startScene) {
	srand((unsigned int)time(NULL));
	engine = _engine;
	player = _player;
	pauseMenu->SetCamera(&player->transform);
	sceneManager = new SceneManager();
	sceneManager->Initialize();
	gameData.Reset();
	hexGrid.Fill(false);

	ChangeScene(startScene);

	//MessageEvents::SendMessage(EVENT_StartWave, EventMessageBase());

	//AStarEnemy *enemy;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<AStarEnemy>("AStarEnemy", { 2,2,2 }, &enemy));
	//enemy->SetGrid(&hexGrid);
	//enemy->SetGoal(DirectX::XMFLOAT2(corePos.x, corePos.z));
	//enemy->Repath();
	//enemy->Enable();
}
void Game::Update() {
	auto playerPos = player->transform.GetMatrix();
	hexGrid.Display(DirectX::XMFLOAT2(playerPos._41, playerPos._43));
	float dt = (float)GhostTime::DeltaTime();

	if (paused) return;

	switch (gameData.state) {
		case GAMESTATE_InWave:
			{
				//--------Spawn Enemies if it's their time
				{
					//For each spawn entry in the level file
					for (int i = 0; i < gameData.waveManager.waves[gameData.waveManager.currentWave].spawns.size(); ++i) {
						//Update this entry's timers
						WaveManager::Wave::Spawners& spawner = gameData.waveManager.waves[gameData.waveManager.currentWave].spawns[i];
						spawner.timeSinceLastSpawn += dt;
						float nextSpawnTime = (!spawner.enemiesSpawned ? spawner.initialDelay : spawner.runDelay);
						//if this entry's timer is ready to go and still has enemies to spawn
						if (spawner.timeSinceLastSpawn >= nextSpawnTime && spawner.enemiesSpawned < spawner.spawnCount) {
							//Spawns enemy at location
							if (gameData.spawners.size() == 0) Console::ErrorLine << "No spawners are in the scene! Wave will be infinite!";
							else if (spawner.spawnerID >= gameData.spawners.size() || spawner.spawnerID < 0) {
								gameData.spawners[rand() % gameData.spawners.size()]->SpawnObject(const_cast<char*>(spawner.enemyName.c_str()), &hexGrid, DirectX::XMFLOAT2(corePos.x, corePos.z));
								spawner.enemiesSpawned++;
								Console::WriteLine << "Spawner Index out of range. Picking random value";
							}
							else {
								gameData.spawners[spawner.spawnerID]->SpawnObject(const_cast<char*>(spawner.enemyName.c_str()), &hexGrid, DirectX::XMFLOAT2(corePos.x, corePos.z));
								spawner.enemiesSpawned++;
							}
							//Reset this entry's timeSinceLastSpawn
							spawner.timeSinceLastSpawn = 0;
						}
					}
				}
				//--------Update Engine Structure
				{
					engine->ExecuteUpdate();
					engine->ExecuteLateUpdate();
				}
			}
			break;
		case GAMESTATE_BetweenWaves:
			{
				//--------Update Engine Structure
				engine->ExecuteUpdate();
				engine->ExecuteLateUpdate();
			}
			break;
		case GAMESTATE_GameOver:
			{
				//--------Update Engine Structure
				engine->ExecuteUpdate();
				engine->ExecuteLateUpdate();
			}
			break;
		case GAMESTATE_SplashScreen:
			{
				//update time
				gameData.timeInScene += dt;

				//if time is up, switch scenes
				if (gameData.sceneTimeLimit > 0) {
					if (gameData.timeInScene >= gameData.sceneTimeLimit) {
						StartEvent();
					}
				}

				//If we have logos to go through
				if (gameData.logos.size()) {
					//if first time
					if (gameData.currentLogoIndex == -1) {
						//Spawn first logo
						if (gameData.logos[++gameData.currentLogoIndex].fileName != "") {
							int id = ObjectFactory::CreatePrefab(&gameData.logos[gameData.currentLogoIndex].fileName);
							MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(id, { 0, 1.5f, 20 }, &gameData.currentLogo));
						}
					}
					else if (gameData.timeInScene >= gameData.logos[gameData.currentLogoIndex].duration) {
						//If this duration is not special, delete it
						if (gameData.logos[gameData.currentLogoIndex].duration != -1) {
							if (gameData.currentLogo) gameData.currentLogo->Destroy();
							gameData.currentLogo = nullptr;
						}
						//If there is a next option
						if (gameData.currentLogoIndex + 1 < gameData.logos.size()) {
							//Update your index to it, and update the duration to be a new timer if not special
							if (gameData.logos[++gameData.currentLogoIndex].duration != -1) {
								gameData.logos[gameData.currentLogoIndex].duration += gameData.timeInScene;
							}
							if (gameData.logos[gameData.currentLogoIndex].fileName != "") {
								int id = ObjectFactory::CreatePrefab(&gameData.logos[gameData.currentLogoIndex].fileName);
								MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(id, { 0, 1.5f, 20 }, &gameData.currentLogo));
							}
						}
						else {
							if (gameData.currentLogo && gameData.logos[gameData.currentLogoIndex].duration != -1) gameData.currentLogo->Destroy();
							while (gameData.logos.size()) {
								gameData.logos.erase(gameData.logos.begin());
							}
							gameData.currentLogo = nullptr;
							if (gameData.sceneTimeLimit == -2) {
								StartEvent();
							}
						}
					}
				}


				engine->ExecuteUpdate();
				engine->ExecuteLateUpdate();
			}
			break;
		case GAMESTATE_MainMenu:
			{
				engine->ExecuteUpdate();
				engine->ExecuteLateUpdate();
			}
			break;
		default:
			Console::ErrorLine << "Invalid Game State Reached!";
			break;
	}
}
void Game::Clean() {
	delete pauseMenu;
	delete mainMenu;
	delete sceneManager;
}