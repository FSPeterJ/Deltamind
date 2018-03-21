#pragma once
class TraitedEnemy;

class Evolver {
	const size_t waveSize;
	const size_t surviveCount; //50%
	const size_t randomCount; //20%
	const size_t traitPoolSampleSize; // 1/numDomTraits
	const size_t traitPoolSize;

	TraitedEnemy* pool;
	TraitedEnemy* aliveTimePool, *damageDeltPool, *damageReceivedPool, *nodesTraversedPool;

	void CreateDominantPools(void);
	void FillDominantPools(void);
	void MutateDominantPools(void);
	void SelectGenesFromDominantPools(void);
	void ConstructPoolWithMutatedGenes(void);
	void RunGeneration(void);
public:
	//void Evolve(size_t generations);
	Evolver(size_t _wave_size);
	~Evolver(void);
};

