#include "GameData.h"
#include "Core.h"
#include "HexGrid.h"
#include "MessageEvents.h"

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
	if (spawnerObjectIndex < 0)
		index = rand() % (int)spawnerObjects.size();
	else
		index = spawnerObjectIndex;
	spawnerObjects[index]->SpawnObject(const_cast<char*>(spawnerData->enemyName.c_str()), grid, _core);
	spawnerData->enemiesSpawned++;
}
void WaveManager::MoveToNextWave() {
	enemiesLeftAlive = waves[++currentWave].enemyCount;
}