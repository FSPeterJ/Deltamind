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

		void Evolver::RunGeneration(void) {
			//i dont like this if check
			if(currentGeneration) {
				PerformSelection(), PerformMutation();
			} else {
				PerformFirstSelection(), PerformFirstMutation();
			}

			Combination();
		}

		size_t Evolver::genToPrune;
		inline bool Evolver::Pruner(const DominantGene& o) {
			return o.generation == genToPrune;
		}


		Evolver::Evolver() : PruneGeneration(&Evolver::Pruner) {
			//I'd like to make this general by passing in policies as varidic...getting domtraits from args...templating this to take more than evolvable... but that's for another time
			AliveTime = [](DominantGene const& a, DominantGene const& b) { return a.performance.results.timeLasted > b.performance.results.timeLasted; };
			DamageDelt = [](DominantGene const& a, DominantGene const& b) { return a.performance.results.damageDelt > b.performance.results.damageDelt; };
			DamageReceived = [](DominantGene const& a, DominantGene const& b) { return a.performance.results.damageReceived < b.performance.results.damageReceived; };
			NodesTraversed = [](DominantGene const& a, DominantGene const& b) { return a.performance.results.nodesTraversed < b.performance.results.nodesTraversed; };
			genepool.reserve(1024);
			testpool.reserve(100); //largest wave size

			aliveTimePool.reserve(70); //100*.5 + 100*.2 (assuming those are the top/rand %s, also not accounting for first wave which would be 140, but first waves are generally small and will fit even if double
			damageDeltPool.reserve(70);
			damageReceivedPool.reserve(70);
			nodesTraversedPool.reserve(70);
		}

		void Evolver::CreateDominantPools(void) {
			aliveTimePool.resize(traitPoolSize);
			damageDeltPool.resize(traitPoolSize);
			damageReceivedPool.resize(traitPoolSize);
			nodesTraversedPool.resize(traitPoolSize);
		}

		//Since this is the first selection, it keeps the parents as generation 0, mutates them as generation 1. This has two gens in one.
		void Evolver::PerformFirstSelection(void) {
			memcpy(&testpool[0], &genepool[0], GetMemAddr(waveSize));

			for(auto it = testpool.begin(), end = testpool.end(); it != end; ++it) { it->generation = currentGeneration; }

			std::random_shuffle(testpool.begin(), testpool.end()); //shuffle the main pool again so I can get a random sample when I copy below

			 size_t s_offset = surviveCount * 2;
			 size_t r_offset = s_offset + randomCount;

			memcpy(&aliveTimePool[0] + s_offset, &testpool[0], GetMemAddr(randomCount)); //The random ones I will mutate
			memcpy(&aliveTimePool[0] + r_offset, &testpool[0], GetMemAddr(randomCount)); //copy random randomCount over to specific pool
			memcpy(&damageDeltPool[0] + s_offset, &testpool[0] + randomCount, GetMemAddr(randomCount));
			memcpy(&damageDeltPool[0] + r_offset, &testpool[0] + randomCount, GetMemAddr(randomCount));
			memcpy(&damageReceivedPool[0] + s_offset, &testpool[0] + randomCount * 2, GetMemAddr(randomCount));
			memcpy(&damageReceivedPool[0] + r_offset, &testpool[0] + randomCount * 2, GetMemAddr(randomCount));
			memcpy(&nodesTraversedPool[0] + s_offset, &testpool[0] + randomCount * 3, GetMemAddr(randomCount));
			memcpy(&nodesTraversedPool[0] + r_offset, &testpool[0] + randomCount * 3, GetMemAddr(randomCount));

			std::sort(testpool.begin(), testpool.end(), AliveTime); //orders the main pool by best surviving times
			memcpy(&aliveTimePool[0], &testpool[0], GetMemAddr(surviveCount)); //copy best surviveCount over to specific pool
			memcpy(&aliveTimePool[0] + surviveCount, &testpool[0], GetMemAddr(surviveCount)); //these ones I will mutate

			std::sort(testpool.begin(), testpool.end(), DamageDelt);
			memcpy(&damageDeltPool[0], &testpool[0], GetMemAddr(surviveCount));
			memcpy(&damageDeltPool[0] + surviveCount, &testpool[0], GetMemAddr(surviveCount));

			std::sort(testpool.begin(), testpool.end(), DamageReceived);
			memcpy(&damageReceivedPool[0], &testpool[0], GetMemAddr(surviveCount));
			memcpy(&damageReceivedPool[0] + surviveCount, &testpool[0], GetMemAddr(surviveCount));

			std::sort(testpool.begin(), testpool.end(), NodesTraversed);
			memcpy(&nodesTraversedPool[0], &testpool[0], GetMemAddr(surviveCount));
			memcpy(&nodesTraversedPool[0] + surviveCount, &testpool[0], GetMemAddr(surviveCount));
		}

		void Evolver::PerformSelection(void) {
			memcpy(&testpool[0], &genepool[0], GetMemAddr(waveSize)); // the first waveSize of genepool were the ones tested

			std::random_shuffle(testpool.begin(), testpool.end()); //shuffle the main pool again so I can get a random sample when I copy below

			memcpy(&aliveTimePool[0] + surviveCount, &testpool[0], GetMemAddr(randomCount)); //copy random randomCount over to specific pool
			memcpy(&damageDeltPool[0] + surviveCount, &testpool[0] + randomCount, GetMemAddr(randomCount));
			memcpy(&damageReceivedPool[0] + surviveCount, &testpool[0] + randomCount * 2, GetMemAddr(randomCount));
			memcpy(&nodesTraversedPool[0] + surviveCount, &testpool[0] + randomCount * 3, GetMemAddr(randomCount));

			std::sort(testpool.begin(), testpool.end(), AliveTime); //orders the main pool by best surviving times
			memcpy(&aliveTimePool[0], &testpool[0], GetMemAddr(surviveCount)); //copy best surviveCount over to specific pool

			std::sort(testpool.begin(), testpool.end(), DamageDelt);
			memcpy(&damageDeltPool[0], &testpool[0], GetMemAddr(surviveCount));

			std::sort(testpool.begin(), testpool.end(), DamageReceived);
			memcpy(&damageReceivedPool[0], &testpool[0], GetMemAddr(surviveCount));

			std::sort(testpool.begin(), testpool.end(), NodesTraversed);
			memcpy(&nodesTraversedPool[0], &testpool[0], GetMemAddr(surviveCount));
		}

		//Since this has two gens, I only want to mutate certain ones (gen 1)
		void Evolver::PerformFirstMutation(void) {
			++currentGeneration;

			testpool.clear(); //clear because we copied both the parents and the to-be-mutated children into the dominant pools
			//not clearing will result in dupe parents for gen 0

			for(size_t i = surviveCount, end = traitPoolSize - randomCount; i < end; ++i) {
				aliveTimePool[i].HandleMutationIteration(CREEP, currentGeneration);
				damageDeltPool[i].HandleMutationIteration(CREEP, currentGeneration);
				damageReceivedPool[i].HandleMutationIteration(CREEP, currentGeneration);
				nodesTraversedPool[i].HandleMutationIteration(CREEP, currentGeneration);
			}
		}

		void Evolver::PerformMutation(void) {
			++currentGeneration;

			testpool.clear();

			//prune generations
			genToPrune = currentGeneration - generationsToKeep;
			genepool.erase(std::remove_if(genepool.begin(), genepool.end(), PruneGeneration), genepool.end());

			combineOffset = genDeathSize; //can probably set combineOffset to the &genDeathSize somewhere to prevent the assignment of this each mutation

			for(size_t i = 0; i < traitPoolSize; ++i) {
				aliveTimePool[i].HandleMutationIteration(CREEP, currentGeneration);
				damageDeltPool[i].HandleMutationIteration(CREEP, currentGeneration);
				damageReceivedPool[i].HandleMutationIteration(CREEP, currentGeneration);
				nodesTraversedPool[i].HandleMutationIteration(CREEP, currentGeneration);
			}

			//for(TraitedEnemy **i = &(aliveTimePool[0]), **end = &(aliveTimePool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(damageDeltPool[0]), **end = &(damageDeltPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(damageReceivedPool[0]), **end = &(damageReceivedPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
			//for(TraitedEnemy **i = &(nodesTraversedPool[0]), **end = &(nodesTraversedPool[traitPoolSize]); i < end; (**(i++)).traits.Mutate(CREEP));
		}

		void Evolver::Combination(void) {
			//it's fine this contains the first gen still because the trait pools have the first gen during the first iteration so they get overwritten
			memcpy(&genepool[0] + combineOffset, &aliveTimePool[0], GetMemAddr(traitPoolSize));
			memcpy(&genepool[0] + combineOffset + traitPoolSize, &damageDeltPool[0], GetMemAddr(traitPoolSize));
			memcpy(&genepool[0] + combineOffset + traitPoolSize * 2, &damageReceivedPool[0], GetMemAddr(traitPoolSize));
			memcpy(&genepool[0] + combineOffset + traitPoolSize * 3, &nodesTraversedPool[0], GetMemAddr(traitPoolSize));

			std::random_shuffle(genepool.begin(), genepool.end());

			//clear dominant pools
			aliveTimePool.clear();
			damageDeltPool.clear();
			damageReceivedPool.clear();
			nodesTraversedPool.clear();
		}

		void Evolver::PrintBestStats(void) {
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
