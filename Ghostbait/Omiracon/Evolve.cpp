#include "stdafx.h"
#include "Evolve.h"
#include <functional>
#include <algorithm>
#include <random>

#include <iostream>
#include <iterator>  
namespace Omiracon {
	namespace Genetics {

		typedef DominantPool::value_type DominantGene;

		using Policy = std::function<bool(DominantGene const&, DominantGene const&)>;

		using Mutation = std::function<void(Traits& traits)>;

		Policy AliveTime;
		Policy DamageDelt;
		Policy DamageReceived;
		Policy NodesTraversed;


		inline size_t GetMemAddr(size_t index) { return index * sizeof(DominantGene); }

		void Evolver::RunGeneration(void) {
			CreateTestSamplePerformanceData();//this simulates what would have happened irl

			FillDominantPools();

			MutateDominantPools();

			SelectGenesFromDominantPools();

			ConstructPoolWithMutatedGenes();
		}

		Evolver::Evolver(std::size_t _wave_size) : traitPoolSize(surviveCount + randomCount), surviveCount(waveSize * 0.5f), randomCount(waveSize * 0.2f), traitPoolSampleSize(waveSize * 0.25f), waveSize(_wave_size) {

			AliveTime = [](DominantGene const& a, DominantGene const& b) { return a.measure.timeLasted > b.measure.timeLasted; };
			DamageDelt = [](DominantGene const& a, DominantGene const& b) { return a.measure.damageDelt > b.measure.damageDelt; };
			DamageReceived = [](DominantGene const& a, DominantGene const& b) { return a.measure.damageReceived < b.measure.damageReceived; };
			NodesTraversed = [](DominantGene const& a, DominantGene const& b) { return a.measure.nodesTraversed < b.measure.nodesTraversed; };

			pool.resize(waveSize);
			for(size_t i = 0; i < waveSize; ++i) {
				pool[i] = TraitedEnemy();
				pool[i].traits.Randormalize();
			}

			//pool = new TraitedEnemy[waveSize]; //creates random pool of chromosomes
			//for(TraitedEnemy *i = &(pool[0]), *end = &(pool[waveSize]); i < end; (*(i++)).traits.Randormalize());

			CreateDominantPools();
		}

		void Evolver::CreateDominantPools(void) {
			//aliveTimePool = new TraitedEnemy*[traitPoolSize];
			//damageDeltPool = new TraitedEnemy*[traitPoolSize];
			//damageReceivedPool = new TraitedEnemy*[traitPoolSize];
			//nodesTraversedPool = new TraitedEnemy*[traitPoolSize];
			aliveTimePool.resize(traitPoolSize);
			damageDeltPool.resize(traitPoolSize);
			damageReceivedPool.resize(traitPoolSize);
			nodesTraversedPool.resize(traitPoolSize);
		}

		void Evolver::CreateTestSamplePerformanceData(void) {
			for(auto&i : pool) i.measure.Reset(), i.CreateSimulatedResults();
			//for(TraitedEnemy *i = &(pool[0]), *end = &(pool[waveSize]); i < end; (*(i++)).CreateSimulatedResults());
		}

		void Evolver::FillDominantPools(void) {
			std::sort(pool.begin(), pool.end(), AliveTime);
			memcpy(&aliveTimePool[0], &pool[0], GetMemAddr(surviveCount)); //copy best surviveCount over
			std::random_shuffle(pool.begin(), pool.end());
			memcpy(&aliveTimePool[0] + surviveCount, &pool[0], GetMemAddr(randomCount)); //copy best surviveCount over

			std::sort(pool.begin(), pool.end(), DamageDelt);
			memcpy(&damageDeltPool[0], &pool[0], GetMemAddr(surviveCount));
			std::random_shuffle(pool.begin(), pool.end());
			memcpy(&damageDeltPool[0] + surviveCount, &pool[0], GetMemAddr(randomCount));

			std::sort(pool.begin(), pool.end(), DamageReceived);
			memcpy(&damageReceivedPool[0], &pool[0], GetMemAddr(surviveCount));
			std::random_shuffle(pool.begin(), pool.end());
			memcpy(&damageReceivedPool[0] + surviveCount, &pool[0], GetMemAddr(randomCount));

			std::sort(pool.begin(), pool.end(), NodesTraversed);
			memcpy(&nodesTraversedPool[0], &pool[0], GetMemAddr(surviveCount));
			std::random_shuffle(pool.begin(), pool.end());
			memcpy(&nodesTraversedPool[0] + surviveCount, &pool[0], GetMemAddr(randomCount));
		}

		void Evolver::MutateDominantPools(void) {
			for(size_t i = 0; i < traitPoolSize; ++i) {
				aliveTimePool[i].traits.Mutate(CREEP);
				damageDeltPool[i].traits.Mutate(CREEP);
				damageReceivedPool[i].traits.Mutate(CREEP);
				nodesTraversedPool[i].traits.Mutate(CREEP);
			}

			//for(TraitedEnemy **i = &(aliveTimePool[0]), **end = &(aliveTimePool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(damageDeltPool[0]), **end = &(damageDeltPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(damageReceivedPool[0]), **end = &(damageReceivedPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(nodesTraversedPool[0]), **end = &(nodesTraversedPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
		}

		void Evolver::SelectGenesFromDominantPools(void) {
			std::random_shuffle(&aliveTimePool[0], &aliveTimePool[0] + traitPoolSize);
			std::random_shuffle(&damageDeltPool[0], &damageDeltPool[0] + traitPoolSize);
			std::random_shuffle(&damageReceivedPool[0], &damageReceivedPool[0] + traitPoolSize);
			std::random_shuffle(&nodesTraversedPool[0], &nodesTraversedPool[0] + traitPoolSize);
		}

		void Evolver::ConstructPoolWithMutatedGenes(void) {
			//delete[] pool;
			//pool = new TraitedEnemy[waveSize];
			//for(size_t i = 0; i < waveSize; ++i) {
			//	delete pool[i];
			//}
			pool.clear();
			pool.resize(waveSize);

			memcpy(&pool[0], &aliveTimePool[0], GetMemAddr(traitPoolSampleSize));
			memcpy(&pool[0] + traitPoolSampleSize, &damageDeltPool[0], GetMemAddr(traitPoolSampleSize));
			memcpy(&pool[0] + traitPoolSampleSize * 2, &damageReceivedPool[0], GetMemAddr(traitPoolSampleSize));
			memcpy(&pool[0] + traitPoolSampleSize * 3, &nodesTraversedPool[0], GetMemAddr(traitPoolSampleSize));
			//std::random_shuffle(&pool[0], &pool[0] + waveSize);
		}

		void Evolver::GetBestStats(void) {
			std::sort(aliveTimePool.begin(), aliveTimePool.end(), AliveTime);
			std::sort(damageDeltPool.begin(), damageDeltPool.end(), DamageDelt);
			std::sort(damageReceivedPool.begin(), damageReceivedPool.end(), DamageReceived);
			std::sort(nodesTraversedPool.begin(), nodesTraversedPool.end(), NodesTraversed);


			std::cout << "The best stats for surviving the longest are: " << aliveTimePool[0].measure.timeLasted << " vs " << aliveTimePool[traitPoolSize - 1].measure.timeLasted << "\n";
			aliveTimePool[0].traits.Print();
			std::cin.get();

			std::cout << "The best stats for most damage delt are: " << damageDeltPool[0].measure.damageDelt << " vs " << damageDeltPool[traitPoolSize - 1].measure.damageDelt << "\n";
			damageDeltPool[0].traits.Print();
			std::cin.get();

			std::cout << "The best stats for taking least damage are: " << damageReceivedPool[0].measure.damageReceived << " vs " << damageReceivedPool[traitPoolSize - 1].measure.damageReceived << "\n";
			damageReceivedPool[0].traits.Print();
			std::cin.get();


			std::cout << "The best stats for traversing least nodes are: " << nodesTraversedPool[0].measure.nodesTraversed << " vs " << nodesTraversedPool[traitPoolSize - 1].measure.nodesTraversed << "\n";
			nodesTraversedPool[0].traits.Print();
			std::cin.get();
		}

		Evolver::~Evolver(void) {
			//for(TraitedEnemy **i = &(aliveTimePool[0]), **end = &(aliveTimePool[traitPoolSize]); i < end; delete(*(i++)));
			//for(TraitedEnemy **i = &(damageDeltPool[0]), **end = &(damageDeltPool[traitPoolSize]); i < end; delete(*(i++)));
			//for(TraitedEnemy **i = &(damageReceivedPool[0]), **end = &(damageReceivedPool[traitPoolSize]); i < end; delete(*(i++)));
			//for(TraitedEnemy **i = &(nodesTraversedPool[0]), **end = &(nodesTraversedPool[traitPoolSize]); i < end; delete(*(i++)));


			//for(size_t i = 0; i < traitPoolSize; ++i) {
			//	delete aliveTimePool[i];
			//	delete damageDeltPool[i];
			//	delete damageReceivedPool[i];
			//	delete nodesTraversedPool[i];
			//}

			//for(size_t i = 0; i < waveSize; ++i) {
			//	delete pool[i];
			//}

			//delete[] aliveTimePool, delete[] damageDeltPool, delete[] damageReceivedPool, delete[] nodesTraversedPool;

		}
	}
}
