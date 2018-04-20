#include <time.h>
#include "Game.h"
#include "Spawner.h"
#include "MessageEvents.h"
#include "GhostTime.h"
#include "PDA.h"
//#include "EngineStructure.h"
#include "../Dependencies/XML_Library/irrXML.h"
#include "AStarEnemy.h"
#include "SceneManager.h"
#include "PathPlanner.h"
#include "AntColony.h"
#include "ObjectFactory.h"
#include "Player.h"
#include "Material.h"
#include "HUD.h"
#include "TargetEnemy.h"
//#include "DStarEnemy.h"
//#include "MTDSLEnemy.h"
#include <future>
#include "Evolvable.h"
#include "CoreShield.h"
using namespace Omiracon::Genetics;

Game::Game() {
	MessageEvents::Subscribe(EVENT_SpawnerCreated, [=](EventMessageBase* e) {this->SpawnerCreatedEvent(e); });
	MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) {this->EnemyDiedEvent(); });
	MessageEvents::Subscribe(EVENT_Start, [=](EventMessageBase* e) {this->StartEvent(e); });
	MessageEvents::Subscribe(EVENT_PauseInputDetected, [=](EventMessageBase* e) {this->PauseInputEvent(); });
	MessageEvents::Subscribe(EVENT_GamePause, [=](EventMessageBase* e) {this->PauseGame(); });
	MessageEvents::Subscribe(EVENT_GameUnPause, [=](EventMessageBase* e) {this->ResumeGame(); });
	MessageEvents::Subscribe(EVENT_GameRestart, [=](EventMessageBase* e) {this->RestartLevel(); });
	MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->RemoveObstacleEvent(e); });
	MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) {this->Lose(); });
	MessageEvents::Subscribe(EVENT_GameQuit, [=](EventMessageBase* e) {this->Quit(); });
	MessageEvents::Subscribe(EVENT_GameExit, [=](EventMessageBase* e) {this->ExitToMainMenu(); });
	MessageEvents::Subscribe(EVENT_FreeMoney, [=](EventMessageBase* e) {this->gameData.SetGears(500000); });
	MessageEvents::Subscribe(EVENT_ChangeScene, [=](EventMessageBase* e) { this->ChangeScene(((ChangeSceneMessage*)e)->RetrieveData()); });
	MessageEvents::Subscribe(EVENT_NextLogo, [=](EventMessageBase* e) { this->NextLogoEvent(); });

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
	if(gameData.GetState() == GAMESTATE_SplashScreen || gameData.GetState() == GAMESTATE_MainMenu || gameData.GetState() == GAMESTATE_Credits) return;
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
void Game::NextLogoEvent() {
	if (gameData.ssManager.NextLogoExists())
		gameData.ssManager.MoveToNextLogo();
	else
		StartEvent();
}
void Game::StartEvent(EventMessageBase* e) {
	StartEventMessage* starter = nullptr;
	if(e) starter = (StartEventMessage*)e;
	switch(gameData.GetState()) {
		case GAMESTATE_SplashScreen:
		{
			splashScreenMenu.Hide();
			std::string levelName = "";
			if (starter)
				levelName = starter->RetrieveLevelName();
			char* sceneName = new char[gameData.ssManager.GetNextScene().length() + 1];
			memcpy(sceneName, gameData.ssManager.GetNextScene().c_str(), gameData.ssManager.GetNextScene().length() + 1);
			ChangeScene(sceneName, levelName);
			delete sceneName;
		}
		break;
		case GAMESTATE_BetweenWaves:
			StartNextWave();
			break;
		case GAMESTATE_MainMenu:
		{
			std::string levelName = "";
			if (starter)
				levelName = starter->RetrieveLevelName();
			ChangeState(GAMESTATE_BetweenWaves);
			ChangeScene("level0", levelName);
			break;
		}
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
					currentTimeBetweenWaveReady = delayBetweenWaveReady;
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
void Game::ChangeScene(const char* sceneName, std::string levelName) {
	//Delete all current scene Items
	MessageEvents::SendMessage(EVENT_DeleteAllGameObjects, EventMessageBase());

	//Reset Game/Wave data
	gameData.Reset();

	//Load scene assets
	sceneManager->LoadScene(sceneName, &core);

	//TODO: TEMPORARY main menu code--------
	if (!strcmp(sceneName, "splashScreen"))
		SplashScreenLoaded();
	if(!strcmp(sceneName, "mainMenu"))
		MainMenuLoaded();
	if (!strcmp(sceneName, "Tutorial"))
		TutorialLoaded();
	if (!strcmp(sceneName, "level0"))
		Level0Loaded();
	if (!strcmp(sceneName, "Credits"))
		CreditsLoaded();

	//--------------------------------------

	//If it has level/wave data, load it
	if (sceneManager->GetCurrentScene().levelFiles.size() > 0) {
		std::string levelFile = std::string("");
		if (levelName == levelFile)
			levelFile = sceneManager->GetCurrentScene().levelFiles[0];
		else
		{
			for (size_t i = 0; i < sceneManager->GetCurrentScene().levelFiles.size(); ++i)
			{
				if (levelName == sceneManager->GetCurrentScene().levelFiles[i])
					levelFile = sceneManager->GetCurrentScene().levelFiles[i];
			}
			if (levelFile.c_str() == "")
				levelFile = sceneManager->GetCurrentScene().levelFiles[0];
		}
		irr::io::IrrXMLReader *xmlReader = irr::io::createIrrXMLReader(levelFile.c_str());
		currLevelName = levelFile;

		WaveManager::Wave* newWave = nullptr;
		while (xmlReader->read()) {
			if (xmlReader->getNodeType() == irr::io::EXN_ELEMENT) {
				if (!strcmp("Level", xmlReader->getNodeName())) {
					gameData.AddGears(xmlReader->getAttributeValueAsInt("startGears"));
					gameData.SetStateHard(GAMESTATE_BetweenWaves);
					gameData.SetPrevStateHard(GAMESTATE_BetweenWaves);
				}
				else if (!strcmp("MenuScene", xmlReader->getNodeName())) {
					gameData.ssManager.SetSceneTimeLimit(xmlReader->getAttributeValueAsFloat("sceneTimeLimit"));
					gameData.ssManager.SetNextScene(xmlReader->getAttributeValue("nextScene"));
					gameData.SetStateHard(GAMESTATE_SplashScreen);
					gameData.SetPrevStateHard(GAMESTATE_SplashScreen);
				}
				else if (!strcmp("Wave", xmlReader->getNodeName())) {
					if (newWave) {
						gameData.waveManager.AddWave(*newWave);
						delete newWave;
					}
					newWave = new WaveManager::Wave();
					(*newWave).reward = xmlReader->getAttributeValueAsInt("reward");
				}
				else if (!strcmp("Spawner", xmlReader->getNodeName())) {
					WaveManager::Wave::SpawnerData newSpawner;
					newSpawner.spawnerID = xmlReader->getAttributeValueAsInt("spawnerID");
					newSpawner.enemyName = xmlReader->getAttributeValueSafe("enemyName");
					newSpawner.spawnCount = xmlReader->getAttributeValueAsInt("spawnCount");
					newSpawner.initialDelay = xmlReader->getAttributeValueAsFloat("initialDelay");
					newSpawner.runDelay = xmlReader->getAttributeValueAsFloat("runDelay");
					(*newWave).spawns.push_back(newSpawner);
					(*newWave).enemyCount += newSpawner.spawnCount;
				}
				else if (!strcmp("Logo", xmlReader->getNodeName())) {
					SplashScreenManager::LogoData logo;
					logo.fileName = xmlReader->getAttributeValue("ghostFile");
					logo.spawnTime = xmlReader->getAttributeValueAsFloat("duration");
					gameData.ssManager.AddLogo(logo);
				}
			}
		}
		if (newWave) {
			gameData.waveManager.AddWave(*newWave);
			delete newWave;
		}
		delete xmlReader;
	}
	else
		currLevelName = "";

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
	ChangeScene(name.c_str(), currLevelName);
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
	//Game* loseCube;
	//unsigned ID = ObjectFactory::CreatePrefab(&std::string("Assets/LoseCube.ghost"));
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuCube>(ID, { 0, 0.75f, 0 }, &loseCube));
	//DirectX::XMFLOAT4X4 newPos;
	//DirectX::XMStoreFloat4x4(&newPos, DirectX::XMLoadFloat4x4(&loseCube->transform.GetMatrix()) * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));
	//loseCube->transform.SetMatrix(newPos);
	//loseCube->Enable();

	ChangeState(GAMESTATE_GameOver);
}
void Game::Win() {
	//Logic to run when the player wins
	MessageEvents::SendMessage(EVENT_GameWin, EventMessageBase());
	//Console::WriteLine << "GAME WAS WON";
	//MenuCube* winCube;
	//unsigned ID = ObjectFactory::CreatePrefab(&std::string("Assets/WinCube.ghost"));
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<MenuCube>(ID, { 0, 0.75f, 0 }, &winCube));
	//winCube->Enable();
}
void Game::Quit() {
	run = false;
}
void Game::ExitToMainMenu() {
	ChangeState(GAMESTATE_MainMenu);
	ChangeScene("mainMenu");
}

//Main Scene Functions
void Game::MainMenuLoaded() {
	worldLight.RemoveLightFromManager();
	//Create Menu
	DirectX::XMFLOAT4X4 menuPos = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1.7f, 2, 1);
	mainMenu.SetSpawnPos(menuPos);
	gameData.SetStateHard(GAMESTATE_MainMenu);
	mainMenu.Show(false);
	//Set Controllers
	player->leftController->ClearInventory();
	player->rightController->ClearInventory();
	player->leftController->SetControllerState(CSTATE_MenuController);
	player->rightController->SetControllerState(player->IsVR() ? CSTATE_MenuController : CSTATE_ModelOnly);

	//Update Player
	player->transform.MoveToOrigin(player->PlayerHeight());
	player->ResetStance();
	player->Teleport(DirectX::XMFLOAT3(0, 0, 0));
	player->transform.LookAt({ menuPos._41, menuPos._42, menuPos._43 });

	//Update HUD
	if (currHUD)
	{
		currHUD->HideInventory();
		currHUD->HideWaveInfo();
	}
}
void Game::TutorialLoaded() {
	worldLight.SetAsDirectional({ 0.25f, 0.25f, 0.25f }, { 0, -0.5f, 0.5f });

	gameData.AddGears(1000);
	//Update Controllers
	{
		int index = 0;
		player->leftController->SetControllerState(CSTATE_Inventory);
		player->rightController->SetControllerState(CSTATE_Inventory);
		player->leftController->ClearInventory();
		player->rightController->ClearInventory();
		//PDA
		if (player->IsVR()) {
			player->leftController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/PDA.ghost")));
			player->rightController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/PDA.ghost")));
			((PDA*)player->leftController->GetItem(index))->SetHand(HAND_Left);
			((PDA*)player->rightController->GetItem(index))->SetHand(HAND_Right);
			++index;
		}
		//Pistol
		player->leftController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/Pistol.ghost")));
		player->rightController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/Pistol.ghost")));
		++index;
		//SMG
		player->leftController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/smgNoStock.ghost")));
		player->rightController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/smgNoStock.ghost")));
		++index;
		//BuildTool
		player->leftController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/BuildTool.ghost")));
		player->rightController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/BuildTool.ghost")));

		player->leftController->SwapItem(index - 2);
		player->rightController->SwapItem(index - 2);

		player->leftController->SetBuildItems({ ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Short.ghost")),
												ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Medium.ghost")),
												ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Long.ghost")) });
		player->rightController->SetBuildItems({ ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Short.ghost")),
												ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Medium.ghost")),
												ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Long.ghost")) });

		player->SetBuildToolData(&hexGrid, &gameData);
	}

	//Update HUD
	if (currHUD)
	{
		currHUD->ShowInventory();
	}
	
	//Spawn Target
	{
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {
			TargetEnemy* targetEnemy;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/TargetEnemy.ghost")), { 14, 0, -10 }, (GameObject**)&targetEnemy));
			targetEnemy->SetRange({ 14, 0, 0 });
			targetEnemy->Enable();
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/TargetEnemy.ghost")), { 14, 0, -10 }, (GameObject**)&targetEnemy));
			targetEnemy->SetRange({ -14, 0, 0 });
			targetEnemy->Enable();
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/TargetEnemy.ghost")), { 14, 0, -20 }, (GameObject**)&targetEnemy));
			targetEnemy->SetRange({ 14, 0, 0 });
			targetEnemy->Enable();
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/TargetEnemy.ghost")), { 14, 0, -20 }, (GameObject**)&targetEnemy));
			targetEnemy->SetRange({ -14, 0, 0 });
			targetEnemy->Enable();
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/TargetEnemy.ghost")), { 14, 0, -30 }, (GameObject**)&targetEnemy));
			targetEnemy->SetRange({ 14, 0, 0 });
			targetEnemy->Enable();
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/TargetEnemy.ghost")), { 14, 0, -30 }, (GameObject**)&targetEnemy));
			targetEnemy->SetRange({ -14, 0, 0 });
			targetEnemy->Enable();
		});
	}

	//Light
	
}
void Game::Level0Loaded() {
	worldLight.RemoveLightFromManager();
	currentTimeBetweenWaveReady = -1;
	int index = 0;
	player->leftController->SetControllerState(CSTATE_Inventory);
	player->rightController->SetControllerState(CSTATE_Inventory);
	player->leftController->ClearInventory();
	player->rightController->ClearInventory();
	//PDA
	if (player->IsVR()) {
		player->leftController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/PDA.ghost")));
		player->rightController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/PDA.ghost")));
		((PDA*)player->leftController->GetItem(index))->SetHand(HAND_Left);
		((PDA*)player->rightController->GetItem(index))->SetHand(HAND_Right);
		++index;
	}
	//Pistol
	player->leftController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/Pistol.ghost")));
	player->rightController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/Pistol.ghost")));
	++index;
	//SMG
	player->leftController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/smgNoStock.ghost")));
	player->rightController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/smgNoStock.ghost")));
	++index;
	//BuildTool
	player->leftController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/BuildTool.ghost")));
	player->rightController->AddItem(index, ObjectFactory::CreatePrefab(&std::string("Assets/BuildTool.ghost")));
	player->leftController->SetBuildItems({ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Short.ghost")),
											ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Medium.ghost")),
											ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Long.ghost")), });
	player->rightController->SetBuildItems({ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Short.ghost")), 
											ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Medium.ghost")), 
											ObjectFactory::CreatePrefab(&std::string("Assets/Turret_Long.ghost")) });

	player->leftController->SwapItem(index - 2);
	player->rightController->SwapItem(index - 2);
	
	player->SetBuildToolData(&hexGrid, &gameData);

	//Update HUD
	if (currHUD)
	{
		currHUD->ShowInventory();
		currHUD->ShowWaveInfo();
	}
}
void Game::CreditsLoaded() {
	worldLight.RemoveLightFromManager();
	player->leftController->SetControllerState(CSTATE_MenuController);
	player->rightController->SetControllerState(CSTATE_MenuController);

	PDA* largePDA = nullptr;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/PDA.ghost")), { 0, 0, 0 }, (GameObject**)&largePDA));
	largePDA->transform.SetMatrix(DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 3, 1));
	largePDA->SetPurpose(PDA::Purpose::Credits);

	creditsMenu.Show();
}
void Game::SplashScreenLoaded() {
	worldLight.SetAsDirectional({ 0.5f, 0.5f, 0.5f }, { 0, 0, 1 });

	//Update Player
	player->transform.MoveToOrigin(player->PlayerHeight());
	player->ResetStance();
	player->Teleport(DirectX::XMFLOAT3(0, 0, 0));
	player->transform.LookAt({ 0, 1.7f, 2 });
	//
	player->leftController->ClearInventory();
	player->rightController->ClearInventory();
	player->leftController->SetControllerState(CSTATE_MenuController);
	player->rightController->SetControllerState(player->IsVR() ? CSTATE_MenuController : CSTATE_ModelOnly);
	splashScreenMenu.Show();
}

//Main loop elements
void Game::Start(Player* _player, EngineStructure* _engine, HUD* _hud, char* startScene, char* xml) {
	srand((unsigned int)time(NULL));
	engine = _engine;
	player = _player;
	currHUD = _hud;
	AntColony::AddUpdateToEngineStruct();
	mainMenu.Create(MENU_Main);
	pauseMenu.Create(MENU_Pause);
	pauseMenu.SetCamera(&player->transform);
	creditsMenu.Create(MENU_Custom, { Button::BUTTON_Exit });
	creditsMenu.SetSpawnPos(DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 3, 1));
	splashScreenMenu.Create(MENU_SplashScreen);
	splashScreenMenu.SetSpawnPos(DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 2, 1));
	sceneManager = new SceneManager();
	sceneManager->Initialize();
	gameData.Reset();
	//hexGrid.Fill(false);
	player->SetBuildToolData(&hexGrid, &gameData);

	ChangeScene(startScene, xml);

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
				std::vector<std::future<bool>> enemiesReady;
				enemiesReady.reserve(64);
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
						//else enemiesReady.push_back(gameData.waveManager.SpawnEnemy(spawner, spawner->spawnerID, &hexGrid, core));
						else {
							gameData.waveManager.SpawnEnemy(spawner, spawner->spawnerID, &hexGrid, core);
						}
						//Reset this entry's timeSinceLastSpawn
						spawner->RestartTimer();
					}
				}

				//Wait for enemies to be ready
				//for (int j = 0; j < enemiesReady.size(); ++j) {
				//	enemiesReady[j].get();
				//}
			}
			
			//--------Update Engine Structure
			{
				engine->ExecuteAnimationUpdate();
				engine->ExecuteUpdate();
				engine->ExecuteLateUpdate();
			}
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
		case GAMESTATE_BetweenWaves:
			{
				if (currentTimeBetweenWaveReady > 0) {
					currentTimeBetweenWaveReady -= dt;
					if (currentTimeBetweenWaveReady <= 0) {
						currentTimeBetweenWaveReady = -1;
						//Replace start cube eventually
						//Spawn start cube
						CoreShield* coreShield;
						unsigned ID = ObjectFactory::CreatePrefab(&std::string("Assets/CoreShield.ghost"));
						MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<CoreShield>(ID, { 0, 0, 0 }, &coreShield));
						coreShield->transform.SetMatrix(core->transform.GetMatrix());
						coreShield->Enable();
					}
				}
			}
		case GAMESTATE_GameOver:
		case GAMESTATE_MainMenu:
		case GAMESTATE_Credits:
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