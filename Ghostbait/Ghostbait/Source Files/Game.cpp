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
	spawners.empty();
	enemiesLeftAlive = 0;
	waveManager.currentWave = -1;
	waveManager.waves.empty();
}

Game::Game() {
	pauseMenu = new Menu(MENU_Pause);
	MessageEvents::Subscribe(EVENT_SpawnerCreated, [=](EventMessageBase* e) {this->SpawnerCreatedEvent(e); });
	MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) {this->EnemyDiedEvent(); });
	MessageEvents::Subscribe(EVENT_StartWave, [=](EventMessageBase* e) {this->StartPressedEvent(); });
	MessageEvents::Subscribe(EVENT_GamePause, [=](EventMessageBase* e) {this->PausePressedEvent(); });
	MessageEvents::Subscribe(EVENT_GameRestart, [=](EventMessageBase* e) {this->RestartGameEvent(); });
	MessageEvents::Subscribe(EVENT_SnapRequest, [=](EventMessageBase* e) {this->SnapRequestEvent(e); });
	MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) {this->AddObstacleEvent(e); });
	MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->RemoveObstacleEvent(e); });
	MessageEvents::Subscribe(EVENT_GameQuit, [=](EventMessageBase* e) {this->QuitEvent(); });
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
void Game::StartPressedEvent() {
	StartNextWave();
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
void Game::RestartGameEvent() {
	RestartLevel();
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
void Game::QuitEvent() {
	run = false;
}

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
		break;
		case GAMESTATE_InWave:
		{
			if (gameData.prevState == GAMESTATE_Paused) ResumeGame();
		}
		break;
		}
	}
}
void Game::ChangeScene(char* sceneName) {
	//Load scene assets
	sceneManager->LoadScene(sceneName);

	//Reset Game/Wave data
	gameData.Reset();

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

void Game::RestartLevel() {
	gameData.Reset();
	sceneManager->ReloadScene();
}
void Game::ResumeGame() {
	//Logic to run when game first gets unPaused
}
void Game::PauseGame() {
	//Logic to run when game first gets paused
}
void Game::Lose() {
	//Logic to run when the player loses
}
void Game::Win() {
	//Logic to run when the player wins
	MessageEvents::SendMessage(EVENT_GameWin, EventMessageBase());
	Console::WriteLine << "GAME WAS WON";									// v THIS WILL PROBABLY CRASH DO NOT DO
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(9/*WinCube*/, { 0, 0.75f, 0 })); 
}

void Game::Start(EngineStructure* _engine, char* startScene) {
	srand((unsigned int)time(NULL));
	engine = _engine;
	sceneManager = new SceneManager();
	sceneManager->Initialize();
	gameData.Reset();
	hexGrid.Fill();

	ChangeScene(startScene);

	//AStarEnemy* fred;
	//MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<AStarEnemy>("AStarEnemy", {0,0,0}, &fred));
	//fred->SetGrid(&hexGrid);
	//fred->Enable();
}
void Game::Update() {
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
								gameData.spawners[rand() % gameData.spawners.size()]->SpawnObject(const_cast<char*>(spawner.enemyName.c_str()), &hexGrid, DirectX::XMFLOAT2(0, 0));
								spawner.enemiesSpawned++;
								Console::WriteLine << "Spawner Index out of range. Picking random value";
							}
							else {
								gameData.spawners[spawner.spawnerID]->SpawnObject(const_cast<char*>(spawner.enemyName.c_str()), &hexGrid, DirectX::XMFLOAT2(0, 0));
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
		default:
			Console::ErrorLine << "Invalid Game State Reached!";
			break;
	}
}
void Game::Clean() {
	delete pauseMenu;
	delete sceneManager;
}
