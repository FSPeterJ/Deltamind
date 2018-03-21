#include "stdafx.h"
#include "Evolve.h"
#include "TraitedEnemy.h"
#include <functional>
#include <algorithm>
#include <random>

using Policy = std::function<bool(TraitedEnemy const&, TraitedEnemy const&)>;

using Mutation = std::function<void(Traits& traits)>;

Policy AliveTime;
Policy DamageDelt;
Policy DamageReceived;
Policy NodesTraversed;

void Evolver::RunGeneration(void) {
	FillDominantPools();

	MutateDominantPools();

	SelectGenesFromDominantPools();

	ConstructPoolWithMutatedGenes();
}

Evolver::Evolver(std::size_t _wave_size) : traitPoolSize(surviveCount + randomCount), surviveCount(waveSize * 0.5f), randomCount(waveSize * 0.2f), traitPoolSampleSize(waveSize * 0.25f), waveSize(_wave_size) {

	AliveTime = [=](TraitedEnemy const& a, TraitedEnemy const& b) { return a.measure.timeLasted < b.measure.timeLasted; };
	DamageDelt = [=](TraitedEnemy const& a, TraitedEnemy const& b) { return a.measure.damageDelt < b.measure.damageDelt; };
	DamageReceived = [=](TraitedEnemy const& a, TraitedEnemy const& b) { return a.measure.damageReceived < b.measure.damageReceived; };
	NodesTraversed = [=](TraitedEnemy const& a, TraitedEnemy const& b) { return a.measure.nodesTraversed < b.measure.nodesTraversed; };

	pool = new TraitedEnemy[waveSize]; //creates random pool of chromosomes

	CreateDominantPools(), FillDominantPools();
}

void Evolver::CreateDominantPools(void) {
	aliveTimePool = new TraitedEnemy[traitPoolSize];
	damageDeltPool = new TraitedEnemy[traitPoolSize];
	damageReceivedPool = new TraitedEnemy[traitPoolSize];
	nodesTraversedPool = new TraitedEnemy[traitPoolSize];
}

void Evolver::FillDominantPools(void) {
	std::sort(pool, pool + waveSize, AliveTime);
	//aliveTimePool = new TraitedEnemy[traitPoolSize];
	memcpy(aliveTimePool, pool, surviveCount);
	std::random_shuffle(pool, pool + waveSize);
	memcpy(aliveTimePool + surviveCount, pool, randomCount);

	std::sort(pool, pool + waveSize, DamageDelt);
	//damageDeltPool = new TraitedEnemy[traitPoolSize];
	memcpy(damageDeltPool, pool, surviveCount);
	std::random_shuffle(pool, pool + waveSize);
	memcpy(damageDeltPool + surviveCount, pool, randomCount);

	std::sort(pool, pool + waveSize, DamageReceived);
	//damageReceivedPool = new TraitedEnemy[traitPoolSize];
	memcpy(damageReceivedPool, pool, surviveCount);
	std::random_shuffle(pool, pool + waveSize);
	memcpy(damageReceivedPool + surviveCount, pool, randomCount);

	std::sort(pool, pool + waveSize, NodesTraversed);
	//nodesTraversedPool = new TraitedEnemy[traitPoolSize];
	memcpy(nodesTraversedPool, pool, surviveCount);
	std::random_shuffle(pool, pool + waveSize);
	memcpy(nodesTraversedPool + surviveCount, pool, randomCount);
}

void Evolver::MutateDominantPools(void) {
	for(TraitedEnemy *i = &(aliveTimePool[0]), *end = &(aliveTimePool[traitPoolSize]); i < end; (*(i++)).traits.Mutate(CREEP));
	for(TraitedEnemy *i = &(damageDeltPool[0]), *end = &(damageDeltPool[traitPoolSize]); i < end; (*(i++)).traits.Mutate(CREEP));
	for(TraitedEnemy *i = &(damageReceivedPool[0]), *end = &(damageReceivedPool[traitPoolSize]); i < end; (*(i++)).traits.Mutate(CREEP));
	for(TraitedEnemy *i = &(nodesTraversedPool[0]), *end = &(nodesTraversedPool[traitPoolSize]); i < end; (*(i++)).traits.Mutate(CREEP));
}

void Evolver::SelectGenesFromDominantPools(void) {
	std::random_shuffle(aliveTimePool, aliveTimePool + traitPoolSize);
	std::random_shuffle(damageDeltPool, damageDeltPool + traitPoolSize);
	std::random_shuffle(damageReceivedPool, damageReceivedPool + traitPoolSize);
	std::random_shuffle(nodesTraversedPool, nodesTraversedPool + traitPoolSize);
}

void Evolver::ConstructPoolWithMutatedGenes(void) {
	delete[] pool;
	pool = new TraitedEnemy[waveSize];

	memcpy(pool, aliveTimePool, traitPoolSampleSize);
	memcpy(pool + traitPoolSampleSize, damageDeltPool, traitPoolSampleSize);
	memcpy(pool + traitPoolSampleSize * 2, damageReceivedPool, traitPoolSampleSize);
	memcpy(pool + traitPoolSampleSize * 3, nodesTraversedPool, traitPoolSampleSize);
}

Evolver::~Evolver(void) {
	delete[] pool, delete[] aliveTimePool, delete[]damageDeltPool, delete[]damageReceivedPool, delete[]nodesTraversedPool;
}
