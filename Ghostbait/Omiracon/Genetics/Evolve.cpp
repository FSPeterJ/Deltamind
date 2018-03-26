#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>  
#include <random>
#include "Evolve.h"

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

		Evolver::Evolver(std::size_t _wave_size, float topPercentage, float randPercentage):
			traitPoolSize(surviveCount + randomCount),
			surviveCount(size_t(waveSize * topPercentage)),
			randomCount(size_t(waveSize * randPercentage)),
			traitPoolSampleSize(size_t(waveSize * (1.0f / DOMINANT_TRAITS))),
			waveSize(_wave_size) {

			AliveTime = [](DominantGene const& a, DominantGene const& b) { return a.measure.timeLasted > b.measure.timeLasted; };
			DamageDelt = [](DominantGene const& a, DominantGene const& b) { return a.measure.damageDelt > b.measure.damageDelt; };
			DamageReceived = [](DominantGene const& a, DominantGene const& b) { return a.measure.damageReceived < b.measure.damageReceived; };
			NodesTraversed = [](DominantGene const& a, DominantGene const& b) { return a.measure.nodesTraversed < b.measure.nodesTraversed; };

			pool.resize(waveSize);
			for(size_t i = 0; i < waveSize; ++i) {//creates random pool of chromosomes
				pool[i] = TraitedEnemy();
				pool[i].traits.Randormalize();
			}

			CreateDominantPools();
		}

		void Evolver::CreateDominantPools(void) {
			aliveTimePool.resize(traitPoolSize);
			damageDeltPool.resize(traitPoolSize);
			damageReceivedPool.resize(traitPoolSize);
			nodesTraversedPool.resize(traitPoolSize);
		}

		void Evolver::CreateTestSamplePerformanceData(void) {
			//this simulates the result of our wave
			for(auto&i : pool) i.measure.Reset(), i.CreateSimulatedResults();
		}

		void Evolver::FillDominantPools(void) {
			std::sort(pool.begin(), pool.end(), AliveTime); //orders the main pool by best surviving times
			memcpy(&aliveTimePool[0], &pool[0], GetMemAddr(surviveCount)); //copy best surviveCount over to specific pool
			std::random_shuffle(pool.begin(), pool.end()); //shuffle the main pool again so I can get a random sample when I copy below
			memcpy(&aliveTimePool[0] + surviveCount, &pool[0], GetMemAddr(randomCount)); //copy random randomCount over to specific pool

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
			//shuffles the pools so when I construct the main pool I get random samples
			std::random_shuffle(&aliveTimePool[0], &aliveTimePool[0] + traitPoolSize);
			std::random_shuffle(&damageDeltPool[0], &damageDeltPool[0] + traitPoolSize);
			std::random_shuffle(&damageReceivedPool[0], &damageReceivedPool[0] + traitPoolSize);
			std::random_shuffle(&nodesTraversedPool[0], &nodesTraversedPool[0] + traitPoolSize);
		}

		void Evolver::ConstructPoolWithMutatedGenes(void) {
			//I don't want anything from the old gene pool anymore because I'm about to repopulate it
			//note I may not want to wipe the entire pool to allow a chance for good traits to stick around
			pool.clear();
			pool.resize(waveSize);

			//repopulate
			memcpy(&pool[0], &aliveTimePool[0], GetMemAddr(traitPoolSampleSize));
			memcpy(&pool[0] + traitPoolSampleSize, &damageDeltPool[0], GetMemAddr(traitPoolSampleSize));
			memcpy(&pool[0] + traitPoolSampleSize * 2, &damageReceivedPool[0], GetMemAddr(traitPoolSampleSize));
			memcpy(&pool[0] + traitPoolSampleSize * 3, &nodesTraversedPool[0], GetMemAddr(traitPoolSampleSize));
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

		Evolver::~Evolver(void) { }
	}
}
