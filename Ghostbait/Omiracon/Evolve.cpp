#include "stdafx.h"
#include "Evolve.h"
#include "TraitedEnemy.h"
#include <functional>
#include <algorithm>

using Policy = std::function<bool(TraitedEnemy const&, TraitedEnemy const&)>;

Evolve::Evolve() {

	Policy AliveTime = [=](TraitedEnemy const& a, TraitedEnemy const& b) { return a.measure.timeLasted < b.measure.timeLasted; };
	Policy DamageDelt = [=](TraitedEnemy const& a, TraitedEnemy const& b) { return a.measure.damageDelt < b.measure.damageDelt; };
	Policy DamageReceived = [=](TraitedEnemy const& a, TraitedEnemy const& b) { return a.measure.damageReceived < b.measure.damageReceived; };
	Policy NodesTraversed = [=](TraitedEnemy const& a, TraitedEnemy const& b) { return a.measure.nodesTraversed < b.measure.nodesTraversed; };

	pool = new TraitedEnemy[waveSize];

	int surviveCount = waveSize * 0.5f; //50%
	int randomCount = waveSize * 0.2f; //20%

	int traitPoolSize = surviveCount + randomCount;

	std::sort(pool, pool + waveSize, AliveTime);
	TraitedEnemy* aliveTimePool = new TraitedEnemy[traitPoolSize];
	memcpy(aliveTimePool, pool, surviveCount);
	std::random_shuffle(pool, pool+waveSize);
	memcpy(aliveTimePool + surviveCount, pool, randomCount);

	std::sort(pool, pool + waveSize, DamageDelt);
	TraitedEnemy* damageDeltPool = new TraitedEnemy[traitPoolSize];
	memcpy(damageDeltPool, pool, surviveCount);
	std::random_shuffle(pool, pool + waveSize);
	memcpy(damageDeltPool + surviveCount, pool, randomCount);

	std::sort(pool, pool + waveSize, DamageReceived);
	TraitedEnemy* damageReceivedPool = new TraitedEnemy[traitPoolSize];
	memcpy(damageReceivedPool, pool, surviveCount);
	std::random_shuffle(pool, pool + waveSize);
	memcpy(damageReceivedPool + surviveCount, pool, randomCount);

	std::sort(pool, pool + waveSize, NodesTraversed);
	TraitedEnemy* nodesTraversedPool = new TraitedEnemy[traitPoolSize];
	memcpy(nodesTraversedPool, pool, surviveCount);
	std::random_shuffle(pool, pool + waveSize);
	memcpy(nodesTraversedPool + surviveCount, pool, randomCount);


	//mutate genes


	int traitPoolSampleSize = waveSize * 0.25f; //1/numDomTraits

	std::random_shuffle(aliveTimePool, aliveTimePool + traitPoolSize);
	std::random_shuffle(damageDeltPool, damageDeltPool + traitPoolSize);
	std::random_shuffle(damageReceivedPool, damageReceivedPool + traitPoolSize);
	std::random_shuffle(nodesTraversedPool, nodesTraversedPool + traitPoolSize);

	
	delete[] pool;
	pool = new TraitedEnemy[waveSize];
	
	memcpy(pool, aliveTimePool, traitPoolSampleSize);
	memcpy(pool + traitPoolSampleSize, damageDeltPool, traitPoolSampleSize);
	memcpy(pool + traitPoolSampleSize * 2, damageReceivedPool, traitPoolSampleSize);
	memcpy(pool + traitPoolSampleSize * 3, nodesTraversedPool, traitPoolSampleSize);

	//pool is now ready to use for next wave
}


Evolve::~Evolve() {}
