#include "Game.h"
#include "Spawner.h"
#include "MessageEvents.h"
#include "GhostTime.h"
#include <time.h>
#include "EngineStructure.h"
#include "Menu.h"
#include "../Dependencies/XML_Library/irrXML.h"
#include "AStarEnemy.h"
#include "VRManager.h"
#include "SceneManager.h"


void Game::GameData::Reset() {
	state = GAMESTATE_BetweenWaves;
	prevState = GAMESTATE_BetweenWaves;
	//for (int i = 0; i < waveManager.waves.size(); ++i) {
	//	for (int j = 0; j < waveManager.waves[i].spawns.size(); ++j) {
	//		waveManager.waves[i].spawns[j].spawnCount = 0;
	//	}
	//}
	enemiesLeftAlive = 100;
	waveManager.currentWave = -1;
	while (spawners.size() > 0) {
		spawners.erase(spawners.begin());
	}
	while (waveManager.waves.size() > 0) {
		waveManager.waves.erase(waveManager.waves.begin());
	}
}

Game::Game() {
	pauseMenu = new Menu(MENU_Pause);
	MessageEvents::Subscribe(EVENT_SpawnerCreated, [=](EventMessageBase* e) {this->SpawnerCreatedEvent(e); });
	MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) {this->EnemyDiedEvent(); });
	MessageEvents::Subscribe(EVENT_StartWave, [=](EventMessageBase* e) {this->StartNextWave(); });
	MessageEvents::Subscribe(EVENT_GamePause, [=](EventMessageBase* e) {this->PausePressedEvent(); });
	MessageEvents::Subscribe(EVENT_GameRestart, [=](EventMessageBase* e) {this->restartNextFrame = true; });
	MessageEvents::Subscribe(EVENT_SnapRequest, [=](EventMessageBase* e) {this->SnapRequestEvent(e); });
	MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) {this->AddObstacleEvent(e); });
	MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->RemoveObstacleEvent(e); });
	MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) {this->Lose(); });
	MessageEvents::Subscribe(EVENT_GameQuit, [=](EventMessageBase* e) {this->Quit(); });
}

//Catch Events
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
void Game::PausePressedEvent() {
	if (gameData.state == GAMESTATE_Paused) {
		ChangeState(gameData.prevState);
		Console::WriteLine << "Game is UnPaused";
	}
	else {
		ChangeState(GAMESTATE_Paused);
		Console::WriteLine << "Game is Paused";
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

//Helpers
void Game::ChangeState(State newState) {
	if (gameData.state != newState) {
		gameData.prevState = gameData.state;
		gameData.state = newState;
		switch (newState) {
		case GAMESTATE_Paused:
			{
				PauseGame();
			}
			break;
		case GAMESTATE_BetweenWaves:
			{
				if (gameData.prevState == GAMESTATE_Paused) ResumeGame();
				else {

					//if upcoming wave doesnt exist...
					int nextWave = gameData.waveManager.currentWave + 1;
					if (nextWave >= gameData.waveManager.waves.size()) {
						Win();
					}
					//if upcoming wave does exist
					else if (gameData.prevState == GAMESTATE_InWave) {
						//Spawn start cube
						MenuCube* startCube;
						MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<MenuCube>("StartCube", { 0, 1.5f, 0.0f }, &startCube));
						DirectX::XMStoreFloat4x4(&startCube->position, DirectX::XMLoadFloat4x4(&startCube->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
						startCube->Enable();
					}
				}
			}
			break;
		case GAMESTATE_InWave:
			{
				if (gameData.prevState == GAMESTATE_Paused) ResumeGame();
			}
			break;
		case GAMESTATE_Lost:
			{
				//gameData.Reset();
			}
			break;
		}
	}
}
void Game::ChangeScene(const char* sceneName, DirectX::XMFLOAT3* _corePos) {
	//Reset Game/Wave data
	gameData.Reset();

	//Load scene assets
	sceneManager->LoadScene(sceneName, _corePos);


	//If it has level/wave data, load it
	if (sceneManager->GetCurrentScene().levelFile != "") {
		irr::io::IrrXMLReader *xmlReader = irr::io::createIrrXMLReader(sceneManager->GetCurrentScene().levelFile.c_str());

		WaveManager::Wave* newWave = nullptr;
		while (xmlReader->read()) {
			if (xmlReader->getNodeType() == irr::io::EXN_ELEMENT) {
				if (!strcmp("Wave", xmlReader->getNodeName())) {
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
			}
		}
		gameData.waveManager.waves.push_back(*newWave);
		delete newWave;
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
	std::string name = sceneManager->GetNameFromScene(sceneManager->UnloadScene());

	//Reinstantiate starting wave values and current scene
	ChangeScene(name.c_str(), &corePos);

}
void Game::ResumeGame() {
	//Logic to run when game first gets unPaused
}
void Game::PauseGame() {
	//Logic to run when game first gets paused
}
void Game::Lose() {
	//Logic to run when the player loses
	MenuCube* loseCube;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<MenuCube>("LoseCube", { 0, 0.75f, 0 }, &loseCube));
	DirectX::XMStoreFloat4x4(&loseCube->position, DirectX::XMLoadFloat4x4(&loseCube->position) * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));
	loseCube->Enable();

	ChangeState(GAMESTATE_Lost);
}
void Game::Win() {
	//Logic to run when the player wins
	MessageEvents::SendMessage(EVENT_GameWin, EventMessageBase());
	Console::WriteLine << "GAME WAS WON";
	MenuCube* winCube;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<MenuCube>("WinCube", { 0, 0.75f, 0 }, &winCube));
	winCube->Enable();
}
void Game::Quit() {
	run = false;
}

//Main loop elements
void Game::Start(EngineStructure* _engine, char* startScene) {
	srand((unsigned int)time(NULL));
	engine = _engine;
	sceneManager = new SceneManager();
	sceneManager->Initialize();
	gameData.Reset();
	hexGrid.Fill();

	ChangeScene(startScene, &corePos);

	//AStarEnemy *enemy;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<AStarEnemy>("AStarEnemy", { 2,2,2 }, &enemy));
	//enemy->SetGrid(&hexGrid);
	//enemy->SetGoal(DirectX::XMFLOAT2(corePos.x, corePos.z));
	//enemy->Repath();
	//enemy->Enable();

	//AStarEnemy* fred;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<AStarEnemy>("AStarEnemy", {0,0,0}, &fred));
	//fred->SetGrid(&hexGrid);
	//fred->Enable();
}
void Game::Update() {

	if (restartNextFrame) {
		RestartLevel();
		restartNextFrame = false;
		return;
	}

	auto playerPos = VRManager::GetInstance().GetPlayerPosition();
	hexGrid.Display(DirectX::XMFLOAT2(playerPos._41, playerPos._43));
	float dt = (float)GhostTime::DeltaTime();
	switch (gameData.state) {
		case GAMESTATE_Paused:
			{
				
			}
			break;
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
		case GAMESTATE_Lost:
			{
				//--------Update Engine Structure
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
	delete sceneManager;
}
