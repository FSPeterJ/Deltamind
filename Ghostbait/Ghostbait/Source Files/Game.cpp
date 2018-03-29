#include <time.h>
#include "Game.h"
#include "Spawner.h"
#include "MessageEvents.h"
#include "GhostTime.h"
//#include "EngineStructure.h"
#include "../Dependencies/XML_Library/irrXML.h"
#include "AStarEnemy.h"
#include "SceneManager.h"
#include "PathPlanner.h"
#include "AntColony.h"
#include "ObjectFactory.h"
#include "Player.h"
//#include "DStarEnemy.h"
//#include "MTDSLEnemy.h"

#include "Evolvable.h"
using namespace Omiracon::Genetics;

Game::Game() {
	MessageEvents::Subscribe(EVENT_SpawnerCreated, [=](EventMessageBase* e) {this->SpawnerCreatedEvent(e); });
	MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) {this->EnemyDiedEvent(); });
	MessageEvents::Subscribe(EVENT_Start, [=](EventMessageBase* e) {this->StartEvent(); });
	MessageEvents::Subscribe(EVENT_PauseInputDetected, [=](EventMessageBase* e) {this->PauseInputEvent(); });
	MessageEvents::Subscribe(EVENT_GamePause, [=](EventMessageBase* e) {this->PauseGame(); });
	MessageEvents::Subscribe(EVENT_GameUnPause, [=](EventMessageBase* e) {this->ResumeGame(); });
	MessageEvents::Subscribe(EVENT_GameRestart, [=](EventMessageBase* e) {this->RestartLevel(); });
	MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->RemoveObstacleEvent(e); });
	MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) {this->Lose(); });
	MessageEvents::Subscribe(EVENT_GameQuit, [=](EventMessageBase* e) {this->Quit(); });
	MessageEvents::Subscribe(EVENT_GameExit, [=](EventMessageBase* e) {this->ExitToMenu(); });
	MessageEvents::Subscribe(EVENT_FreeMoney, [=](EventMessageBase* e) {this->gameData.SetGears(500000); });

	MessageEvents::Subscribe(EVENT_GameDataRequest, [=](EventMessageBase* e) { this->GameDataRequestEvent(e); });
	PathPlanner::SetGrid(&hexGrid);
	AntColony::SetGrid(&hexGrid);
	//gameData = GameData(&evolver);
}

//Catch Events
void Game::GameDataRequestEvent(EventMessageBase* e) {
	GameDataMessage* message = (GameDataMessage*)e;
	GameData const** data = message->RetrieveData();
	(*data) = &gameData;
}
void Game::SpawnerCreatedEvent(EventMessageBase* e) {
	SpawnerObject* spawner = dynamic_cast<SpawnerObject*>(((SpawnerCreatedMessage*)e)->RetrieveObject());
	if(spawner)
		gameData.waveManager.AddSpawnerObject(spawner);
	else
		Console::ErrorLine << "A non-spawner Object was send to Game.h in the SpawnerCreated Message!";
}
void Game::RemoveObstacleEvent(EventMessageBase* e) {
	SnapMessage* message = (SnapMessage*)e;
	hexGrid.RemoveObstacle(*message->position);
}
void Game::PauseInputEvent() {
	if(gameData.GetState() == GAMESTATE_SplashScreen || gameData.GetState() == GAMESTATE_MainMenu) return;
	if(paused) {
		ResumeGame();
		MessageEvents::SendMessage(EVENT_GameUnPause, EventMessageBase());
	}
	else {
		PauseGame();
		MessageEvents::SendMessage(EVENT_GamePause, EventMessageBase());
	}
}
void Game::EnemyDiedEvent() {
	if(gameData.GetState() == GAMESTATE_GameOver) return;
	gameData.waveManager.EnemyKilled();
	gameData.AddGears(50);
	if(gameData.waveManager.GetAliveEnemyCount() <= 0) {
		evolver.RunGeneration();
		ChangeState(GAMESTATE_BetweenWaves);
	}
}
void Game::StartEvent() {
	switch(gameData.GetState()) {
		case GAMESTATE_SplashScreen:
		{
			char* sceneName = new char[gameData.ssManager.GetNextScene().length() + 1];
			memcpy(sceneName, gameData.ssManager.GetNextScene().c_str(), gameData.ssManager.GetNextScene().length() + 1);
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
	if(gameData.GetState() != newState) {
		gameData.UpdateState(newState);

		switch(newState) {
			case GAMESTATE_BetweenWaves:
			{
				//if upcoming wave doesnt exist...
				if(!gameData.waveManager.NextWaveExists()) {
					Win();
				}
				//if upcoming wave does exist
				else if(gameData.GetPrevState() == GAMESTATE_InWave) {
					//Add wave reward
					gameData.AddGears(gameData.waveManager.GetCurrentWaveReward());
					GameData const* gd = &gameData;
					MessageEvents::SendMessage(EVENT_WaveComplete, GameDataMessage(&gd));

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
	sceneManager->LoadScene(sceneName, &core);

	//TODO: TEMPORARY main menu code--------
	if(!strcmp(sceneName, "mainMenu")) {
		DirectX::XMFLOAT4X4 menuPos = DirectX::XMFLOAT4X4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 1.7f, 2, 1);
		gameData.SetStateHard(GAMESTATE_MainMenu);
		mainMenu.SetSpawnPos(menuPos);
		mainMenu.Show(false);
		player->leftController->SetControllerState(CSTATE_MenuController);
		player->rightController->SetControllerState(player->IsVR() ? CSTATE_MenuController : CSTATE_ModelOnly);
		player->transform.MoveToOrigin(player->PlayerHeight());
		player->ResetStance();
		DirectX::XMFLOAT3 temp = DirectX::XMFLOAT3(0, 0, 0);
		player->Teleport(&temp);
		player->transform.LookAt({ menuPos._41, menuPos._42, menuPos._43 });
	}

	//--------------------------------------

	//If it has level/wave data, load it
	if(sceneManager->GetCurrentScene().levelFiles.size() > 0) {
		irr::io::IrrXMLReader *xmlReader = irr::io::createIrrXMLReader(sceneManager->GetCurrentScene().levelFiles[0].c_str());

		WaveManager::Wave* newWave = nullptr;
		while(xmlReader->read()) {
			if(xmlReader->getNodeType() == irr::io::EXN_ELEMENT) {
				if(!strcmp("Level", xmlReader->getNodeName())) {
					gameData.AddGears(xmlReader->getAttributeValueAsInt("startGears"));
					gameData.SetStateHard(GAMESTATE_BetweenWaves);
					gameData.SetPrevStateHard(GAMESTATE_BetweenWaves);
					player->leftController->SetControllerState(CSTATE_Inventory);
					player->rightController->SetControllerState(CSTATE_Inventory);
				}
				else if(!strcmp("MenuScene", xmlReader->getNodeName())) {
					gameData.ssManager.SetSceneTimeLimit(xmlReader->getAttributeValueAsFloat("sceneTimeLimit"));
					gameData.ssManager.SetNextScene(xmlReader->getAttributeValue("nextScene"));
					gameData.SetStateHard(GAMESTATE_SplashScreen);
					gameData.SetPrevStateHard(GAMESTATE_SplashScreen);
					player->leftController->SetControllerState(CSTATE_ModelOnly);
					player->rightController->SetControllerState(CSTATE_ModelOnly);
				}
				else if(!strcmp("Wave", xmlReader->getNodeName())) {
					if(newWave) {
						gameData.waveManager.AddWave(*newWave);
						delete newWave;
					}
					newWave = new WaveManager::Wave();
					(*newWave).reward = xmlReader->getAttributeValueAsInt("reward");
				}
				else if(!strcmp("Spawner", xmlReader->getNodeName())) {
					WaveManager::Wave::SpawnerData newSpawner;
					newSpawner.spawnerID = xmlReader->getAttributeValueAsInt("spawnerID");
					newSpawner.enemyName = xmlReader->getAttributeValueSafe("enemyName");
					newSpawner.spawnCount = xmlReader->getAttributeValueAsInt("spawnCount");
					newSpawner.initialDelay = xmlReader->getAttributeValueAsFloat("initialDelay");
					newSpawner.runDelay = xmlReader->getAttributeValueAsFloat("runDelay");
					(*newWave).spawns.push_back(newSpawner);
					(*newWave).enemyCount += newSpawner.spawnCount;
				}
				else if(!strcmp("Logo", xmlReader->getNodeName())) {
					SplashScreenManager::LogoData logo;
					logo.fileName = xmlReader->getAttributeValue("ghostFile");
					logo.spawnTime = xmlReader->getAttributeValueAsFloat("duration");
					gameData.ssManager.AddLogo(logo);
				}
			}
		}
		if(newWave) {
			gameData.waveManager.AddWave(*newWave);
			delete newWave;
		}
		delete xmlReader;
	}


}
void Game::StartNextWave() {
	if(!gameData.waveManager.NextWaveExists()) {
		Console::ErrorLine << "You tried to progress to a wave that doesnt exist!";
	}
	else {
		gameData.waveManager.MoveToNextWave();
		GameData const* gd = &gameData;
		MessageEvents::SendMessage(EVENT_WaveChange, GameDataMessage(&gd));
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
	if(!paused) return;
	paused = false;
	player->leftController->SetControllerStateToPrevious();
	player->rightController->SetControllerStateToPrevious();
}
void Game::PauseGame() {
	//Logic to run when game first gets paused
	if(paused) return;
	paused = true;
	player->leftController->SetControllerState(CSTATE_MenuController);
	player->rightController->SetControllerState(player->IsVR() ? CSTATE_MenuController : CSTATE_ModelOnly);
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
	ChangeState(GAMESTATE_MainMenu);
	ChangeScene("mainMenu");
}

//Main loop elements
void Game::Start(Player* _player, EngineStructure* _engine, char* startScene) {
	srand((unsigned int)time(NULL));
	engine = _engine;
	player = _player;
	AntColony::AddUpdateToEngineStruct();
	mainMenu.Create(MENU_Main);
	pauseMenu.Create(MENU_Pause);
	pauseMenu.SetCamera(&player->transform);
	sceneManager = new SceneManager();
	sceneManager->Initialize();
	gameData.Reset();
	//hexGrid.Fill(false);
	player->SetBuildToolData(&hexGrid, &gameData);

	ChangeScene(startScene);

	//MessageEvents::SendMessage(EVENT_StartWave, EventMessageBase());
	//DStarEnemy* newFred;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<DStarEnemy>("DStarEnemy", {40,0,40}, &newFred));
	//newFred->SetGrid(&hexGrid);
	//newFred->SetGoal(DirectX::XMFLOAT2(corePos.x, corePos.z));
	//newFred->Enable();
	//AStarEnemy *enemy;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<AStarEnemy>("AStarEnemy", { 2,2,2 }, &enemy));
	//enemy->SetGrid(&hexGrid);
	//enemy->SetGoal(DirectX::XMFLOAT2(corePos.x, corePos.z));
	//enemy->Repath();
	//enemy->Enable();
	//MTDSLEnemy* ted;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<MTDSLEnemy>("MTDSLEnemy", { 5,0,5 }, &ted));
	//ted->SetGrid(&hexGrid);
	//ted->SetGoalReference(&(player->transform.matrix));
	//ted->Enable();
}
void Game::Update() {
	auto playerPos = player->transform.GetMatrix();
	//hexGrid.Display(DirectX::XMFLOAT2(playerPos._41, playerPos._43));
	float dt = (float)GhostTime::DeltaTime();

	if(paused) return;

	switch(gameData.GetState()) {
		case GAMESTATE_InWave:
		{
			//--------Spawn Enemies if it's their time
			{
				//For each spawn entry in the level file
				for(int i = 0; i < gameData.waveManager.GetSpawnCount(); ++i) {
					//Update this entry's timers
					WaveManager::Wave::SpawnerData* spawner = gameData.waveManager.GetSpawnerData(i);
					spawner->UpdateTimer(dt);
					float nextSpawnTime = (!spawner->enemiesSpawned ? spawner->initialDelay : spawner->runDelay);

					//if this entry's timer is ready to go and still has enemies to spawn
					if(spawner->ReadyToSpawn(nextSpawnTime) && spawner->EnemiesToSpawn()) {
						//Spawns enemy at location
						if(gameData.waveManager.GetSpawnObjectCount() == 0) Console::ErrorLine << "No spawners are in the scene! Wave will be infinite!";
						else gameData.waveManager.SpawnEnemy(spawner, spawner->spawnerID, &hexGrid, core);
						//Reset this entry's timeSinceLastSpawn
						spawner->RestartTimer();
					}
				}

			}
			//--------Update Engine Structure
			{
				engine->ExecuteAnimationUpdate();
				engine->ExecuteUpdate();
				engine->ExecuteLateUpdate();
			}
		}
		break;
		case GAMESTATE_BetweenWaves:
		{
			//--------Update Engine Structure
			engine->ExecuteAnimationUpdate();
			engine->ExecuteUpdate();
			engine->ExecuteLateUpdate();
		}
		break;
		case GAMESTATE_GameOver:
		{
			//--------Update Engine Structure
			engine->ExecuteAnimationUpdate();
			engine->ExecuteUpdate();
			engine->ExecuteLateUpdate();
		}
		break;
		case GAMESTATE_SplashScreen:
		{
			//update time
			gameData.ssManager.UpdateTimeInScene(dt);

			//if time is up, switch scenes
			if(gameData.ssManager.HasTimeLimit()) {
				if(gameData.ssManager.TimeLimitReached()) {
					StartEvent();
				}
			}

			//If we have logos to go through
			if(gameData.ssManager.GetLogoCount()) {
				//if first time
				if(gameData.ssManager.GetCurrentLogoIndex() == -1) {
					//Spawn first logo
					gameData.ssManager.MoveToNextLogo();
					if(gameData.ssManager.GetCurrentLogoData().fileName != "") {
						int id = ObjectFactory::CreatePrefab(&gameData.ssManager.GetCurrentLogoData().fileName);
						GameObject* newLogo;
						MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(id, { 0, 1.5f, 20 }, &newLogo));
						gameData.ssManager.SetCurrentLogoObject(newLogo);
					}
				}
				else if(gameData.ssManager.TimeToSpawnNextLogo()) {
					//If this duration is not special, delete it
					if(gameData.ssManager.GetCurrentLogoData().spawnTime != -1) {
						if(gameData.ssManager.GetCurrentLogoObject()) gameData.ssManager.GetCurrentLogoObject()->Destroy();
						gameData.ssManager.SetCurrentLogoObject(nullptr);
					}
					//If there is a next option
					if(gameData.ssManager.NextLogoExists()) {
						//Update your index to it, and update the duration to be a new timer if not special
						gameData.ssManager.MoveToNextLogo();
						if(gameData.ssManager.GetCurrentLogoData().spawnTime != -1) {
							gameData.ssManager.UpdateCurrentLogoSpawnTime();
						}
						if(gameData.ssManager.GetCurrentLogoData().fileName != "") {
							int id = ObjectFactory::CreatePrefab(&gameData.ssManager.GetCurrentLogoData().fileName);
							GameObject* newLogo;
							MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(id, { 0, 1.5f, 20 }, &newLogo));
							gameData.ssManager.SetCurrentLogoObject(newLogo);
						}
					}
					else {
						if(gameData.ssManager.GetCurrentLogoObject() && gameData.ssManager.GetCurrentLogoData().spawnTime != -1) gameData.ssManager.GetCurrentLogoObject()->Destroy();
						gameData.ssManager.ClearLogos();
						gameData.ssManager.SetCurrentLogoObject(nullptr);
						if(gameData.ssManager.TimeLimitReached()) {
							StartEvent();
						}
					}
				}
			}


			engine->ExecuteAnimationUpdate();
			engine->ExecuteUpdate();
			engine->ExecuteLateUpdate();
		}
		break;
		case GAMESTATE_MainMenu:
		{
			engine->ExecuteAnimationUpdate();
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
	delete sceneManager;
}
