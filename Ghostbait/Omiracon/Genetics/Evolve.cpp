#include <algorithm>
#include <functional>
#include <iostream>
#include "Evolve.h"
#include "Evolvable.h"

namespace Omiracon {
	namespace Genetics {
		using DominantGene = Evolver::DominantGene;

		using Policy = std::function<bool(DominantGene const&, DominantGene const&)>;
		using Mutation = std::function<void(Traits& traits)>;

		Policy AliveTime;
		Policy DamageDelt;
		Policy DamageReceived;
		Policy NodesTraversed;

		inline size_t GetMemAddr(size_t index) { return index * sizeof(DominantGene); }

		void Evolver::RunGeneration(void) {
			//CreateTestSamplePerformanceData();//this simulates what would have happened irl

			//i dont like this if check
			if(currentGeneration) {
				PerformSelection();
				PerformMutation();
			} else {
				PerformFirstSelection();
				PerformFirstMutation();
			}
		
			//SelectGenesFromDom();
			Combination();
		}

		Evolver::Evolver(const size_t base_size, const float _topPercentage, const float _randPercentage) {
			AliveTime = [](DominantGene const& a, DominantGene const& b) { return a.performance.results.timeLasted > b.performance.results.timeLasted; };
			DamageDelt = [](DominantGene const& a, DominantGene const& b) { return a.performance.results.damageDelt > b.performance.results.damageDelt; };
			DamageReceived = [](DominantGene const& a, DominantGene const& b) { return a.performance.results.damageReceived < b.performance.results.damageReceived; };
			NodesTraversed = [](DominantGene const& a, DominantGene const& b) { return a.performance.results.nodesTraversed < b.performance.results.nodesTraversed; };

			pool.reserve(base_size*4);

			aliveTimePool.reserve(base_size);
			damageDeltPool.reserve(base_size);
			damageReceivedPool.reserve(base_size);
			nodesTraversedPool.reserve(base_size);
		}

		void Evolver::CreateDominantPools(void) {
			aliveTimePool.resize(traitPoolSize);
			damageDeltPool.resize(traitPoolSize);
			damageReceivedPool.resize(traitPoolSize);
			nodesTraversedPool.resize(traitPoolSize);
		}

		void Evolver::CreateTestSamplePerformanceData(void) {
			//this simulates the result of our wave
			//for(auto&i : pool) i.measure.Reset(), i.CreateSimulatedResults();
		}

		void Evolver::PerformFirstSelection(void) {
			std::sort(pool.begin(), pool.end(), AliveTime); //orders the main pool by best surviving times
			memcpy(&aliveTimePool[0], &pool[0], GetMemAddr(surviveCount)); //copy best surviveCount over to specific pool
			memcpy(&aliveTimePool[0]+ surviveCount, &pool[0], GetMemAddr(surviveCount)); //these ones I will mutate
			std::random_shuffle(pool.begin(), pool.end()); //shuffle the main pool again so I can get a random sample when I copy below
			memcpy(&aliveTimePool[0] + surviveCount*2, &pool[0], GetMemAddr(randomCount)); //I will mutate these ones too
			memcpy(&aliveTimePool[0] + surviveCount*2+ randomCount, &pool[0], GetMemAddr(randomCount)); //copy random randomCount over to specific pool

			std::sort(pool.begin(), pool.end(), DamageDelt);
			memcpy(&damageDeltPool[0], &pool[0], GetMemAddr(surviveCount));
			memcpy(&damageDeltPool[0] + surviveCount, &pool[0], GetMemAddr(surviveCount));
			std::random_shuffle(pool.begin(), pool.end());
			memcpy(&damageDeltPool[0] + surviveCount*2, &pool[0], GetMemAddr(randomCount));
			memcpy(&damageDeltPool[0] + surviveCount * 2 + randomCount, &pool[0], GetMemAddr(randomCount));

			std::sort(pool.begin(), pool.end(), DamageReceived);
			memcpy(&damageReceivedPool[0], &pool[0], GetMemAddr(surviveCount));
			memcpy(&damageReceivedPool[0] + surviveCount, &pool[0], GetMemAddr(surviveCount));
			std::random_shuffle(pool.begin(), pool.end());
			memcpy(&damageReceivedPool[0] + surviveCount*2, &pool[0], GetMemAddr(randomCount));
			memcpy(&damageReceivedPool[0] + surviveCount * 2 + randomCount, &pool[0], GetMemAddr(randomCount));

			std::sort(pool.begin(), pool.end(), NodesTraversed);
			memcpy(&nodesTraversedPool[0], &pool[0], GetMemAddr(surviveCount));
			memcpy(&nodesTraversedPool[0] + surviveCount, &pool[0], GetMemAddr(surviveCount));
			std::random_shuffle(pool.begin(), pool.end());
			memcpy(&nodesTraversedPool[0] + surviveCount*2, &pool[0], GetMemAddr(randomCount));
			memcpy(&nodesTraversedPool[0] + surviveCount * 2 + randomCount, &pool[0], GetMemAddr(randomCount));
		}

		void Evolver::PerformSelection(void) {
			//prune generations
			pool.erase( std::remove_if(pool.begin(), pool.end(), [&](const DominantGene & o) { return o.generation == currentGeneration - generationsToKeep; }), pool.end());

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

		void Evolver::PerformFirstMutation(void) {
			//exectues traitPoolSize*0.5 times
			++currentGeneration;
			for(size_t i = surviveCount; i < traitPoolSize-randomCount; ++i) {
				aliveTimePool[i].traits.Mutate(CREEP);
				damageDeltPool[i].traits.Mutate(CREEP);
				damageReceivedPool[i].traits.Mutate(CREEP);
				nodesTraversedPool[i].traits.Mutate(CREEP);

				aliveTimePool[i].generation= currentGeneration;
				damageDeltPool[i].generation = currentGeneration;
				damageReceivedPool[i].generation = currentGeneration;
				nodesTraversedPool[i].generation = currentGeneration;
			}
		}

		void Evolver::PerformMutation(void) {
			++currentGeneration;
			for(size_t i = 0; i < traitPoolSize; ++i) {
				aliveTimePool[i].traits.Mutate(CREEP);
				damageDeltPool[i].traits.Mutate(CREEP);
				damageReceivedPool[i].traits.Mutate(CREEP);
				nodesTraversedPool[i].traits.Mutate(CREEP);

				aliveTimePool[i].generation = currentGeneration;
				damageDeltPool[i].generation = currentGeneration;
				damageReceivedPool[i].generation = currentGeneration;
				nodesTraversedPool[i].generation = currentGeneration;
			}

			//for(TraitedEnemy **i = &(aliveTimePool[0]), **end = &(aliveTimePool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(damageDeltPool[0]), **end = &(damageDeltPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(damageReceivedPool[0]), **end = &(damageReceivedPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(nodesTraversedPool[0]), **end = &(nodesTraversedPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
		}


		void Evolver::Combination(void) {

			//If you want to copy over a sample instead of the entire pool, uncomment below lines
			//shuffles the pools so when I construct the main pool I get random samples
			//std::random_shuffle(&aliveTimePool[0], &aliveTimePool[0] + traitPoolSize);
			//std::random_shuffle(&damageDeltPool[0], &damageDeltPool[0] + traitPoolSize);
			//std::random_shuffle(&damageReceivedPool[0], &damageReceivedPool[0] + traitPoolSize);
			//std::random_shuffle(&nodesTraversedPool[0], &nodesTraversedPool[0] + traitPoolSize);

			//repopulate
			//memcpy(&pool[0], &aliveTimePool[0], GetMemAddr(traitPoolSampleSize));
			//memcpy(&pool[0] + traitPoolSampleSize, &damageDeltPool[0], GetMemAddr(traitPoolSampleSize));
			//memcpy(&pool[0] + traitPoolSampleSize * 2, &damageReceivedPool[0], GetMemAddr(traitPoolSampleSize));
			//memcpy(&pool[0] + traitPoolSampleSize * 3, &nodesTraversedPool[0], GetMemAddr(traitPoolSampleSize));

			//if you want to copy over the entire pool instead of a sample, use below
			memcpy(&pool[0], &aliveTimePool[0], GetMemAddr(traitPoolSize));
			memcpy(&pool[0] + traitPoolSize, &damageDeltPool[0], GetMemAddr(traitPoolSize));
			memcpy(&pool[0] + traitPoolSize * 2, &damageReceivedPool[0], GetMemAddr(traitPoolSize));
			memcpy(&pool[0] + traitPoolSize * 3, &nodesTraversedPool[0], GetMemAddr(traitPoolSize));

			//clear dominant pools
			aliveTimePool.clear();
			damageDeltPool.clear();
			damageReceivedPool.clear();
			nodesTraversedPool.clear();
		}

		void Evolver::GetTraits() {

		}

		void Evolver::GetBestStats(void) {
			std::sort(aliveTimePool.begin(), aliveTimePool.end(), AliveTime);
			std::sort(damageDeltPool.begin(), damageDeltPool.end(), DamageDelt);
			std::sort(damageReceivedPool.begin(), damageReceivedPool.end(), DamageReceived);
			std::sort(nodesTraversedPool.begin(), nodesTraversedPool.end(), NodesTraversed);

			std::cout << "The best stats for surviving the longest are: " << aliveTimePool[0].performance.results.timeLasted << " vs " << aliveTimePool[traitPoolSize - 1].performance.results.timeLasted << "\n";
			aliveTimePool[0].traits.Print();
			std::cin.get();

			std::cout << "The best stats for most damage delt are: " << damageDeltPool[0].performance.results.damageDelt << " vs " << damageDeltPool[traitPoolSize - 1].performance.results.damageDelt << "\n";
			damageDeltPool[0].traits.Print();
			std::cin.get();

			std::cout << "The best stats for taking least damage are: " << damageReceivedPool[0].performance.results.damageReceived << " vs " << damageReceivedPool[traitPoolSize - 1].performance.results.damageReceived << "\n";
			damageReceivedPool[0].traits.Print();
			std::cin.get();

			std::cout << "The best stats for traversing least nodes are: " << nodesTraversedPool[0].performance.results.nodesTraversed << " vs " << nodesTraversedPool[traitPoolSize - 1].performance.results.nodesTraversed << "\n";
			nodesTraversedPool[0].traits.Print();
			std::cin.get();
		}

		Evolver::~Evolver(void) {}
	}
}
