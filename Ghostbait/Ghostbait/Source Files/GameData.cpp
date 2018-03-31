#include "GameData.h"
#include "Core.h"
#include "HexGrid.h"
#include "MessageEvents.h"
#include "ThreadPool.h"
#include "Evolve.h"
using namespace Omiracon::Genetics;

#include "EnemyBase.h"

bool WaveManager::firstWave = true;


void GameData::Reset() {
	state = GAMESTATE_BetweenWaves;
	prevState = GAMESTATE_BetweenWaves;
	gears = 0;
	turretsSpawned = 0;
	maxTurrets = 100;

	waveManager.Reset();
	ssManager.Reset();
}
void GameData::SetGears(const unsigned amount) {
	gears = amount;
	GameData const* gameData = this;
	MessageEvents::SendMessage(EVENT_GearChange, GameDataMessage(&gameData));
}
void GameData::AddGears(const int amount) {
	SetGears(gears + amount);
}
void GameData::UpdateState(const State& newState) {
	if (state != newState) {
		prevState = state;
		state = newState;
	}
}
void WaveManager::Reset() {
	while (spawnerObjects.size() > 0) {
		spawnerObjects.erase(spawnerObjects.begin()); //can we not use .clear?
	}
	enemiesLeftAlive = 10000;
	while (waves.size() > 0) {
		waves.erase(waves.begin()); //can we not use .clear?
	}
	currentWave = -1;
}
void SplashScreenManager::Reset() {
	nextScene = "";
	timeInScene = 0;
	sceneTimeLimit = -1;
	currentLogoIndex = -1;
	currentLogo = nullptr;
	while (logos.size()) {
		logos.erase(logos.begin()); //can we not use .clear?
	}
}

void WaveManager::SpawnEnemy(WaveManager::Wave::SpawnerData* spawnerData, const int& spawnerObjectIndex, HexGrid* grid, Core* _core) {
	int index = 0;
	if(spawnerObjectIndex < 0) {
		index = rand() % (int) spawnerObjects.size();
	} else {
		index = spawnerObjectIndex;
	}
	spawnerData->enemiesSpawned++;
	auto enemy = spawnerObjects[index]->SpawnObject(spawnerData->enemyName.c_str(), grid, _core);
	enemy->SetTraits() = &evolver->GenePool()[waves[currentWave].numberOfEnemiesSpawned++]; //hopefully this wont break... if it does that means the pool does not have enough in it to account for the number of enemies spawned. which case we would randomly sample the existing pool until we have traits for each enemy. But that wouldnt work because each enemy has a poitner to the traits inside the genepool and random sampleing would cause the random ones to overwrite the prformance data of the enemies that have it first.. so.. dont have this crash.. there'd need to be a expand in genetics that makes new objects thats a copy of existing
	enemy->Start();
}
void WaveManager::MoveToNextWave() {
	enemiesLeftAlive = waves[++currentWave].enemyCount;
	if (currentWave) {
		evolver->SetWaveSize(enemiesLeftAlive);
	} else {
		evolver->SetFirstWave(enemiesLeftAlive);
	}
}
