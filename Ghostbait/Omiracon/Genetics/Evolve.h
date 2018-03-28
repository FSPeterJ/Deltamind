#pragma once

#include <vector>
#include "Evolvable.h"

namespace Omiracon {
	namespace Genetics {
		class Evolver {
		public:
			typedef std::vector<Evolvable> DominantPool;
			typedef DominantPool::value_type DominantGene;

			DominantPool &const GenePool() { return pool; }

			void RunGeneration(void);
			void GetBestStats(void);
			//void Evolve(size_t generations);
			Evolver(const size_t base_size, const float _topPercentage = 0.5f, const float _randPercentage = 0.2f);
			~Evolver(void);

			//template <class T> void Evolve(void);

			void SetFirstWave(const size_t wave_size, const float _topPercentage = 0.5f, const float _randPercentage = 0.2f) {
				topPercentage = _topPercentage;
				randPercentage = _randPercentage;
				//traitPoolSampleSize = (size_t(wave_size * (1.0f / DOMINANT_TRAITS)));
				surviveCount = (size_t(wave_size * topPercentage));
				randomCount = (size_t(wave_size * randPercentage));
				traitPoolSize = 2*(surviveCount + randomCount);
				waveSize = (4* traitPoolSize);

				pool.resize(waveSize);
				for(size_t i = 0; i < waveSize; ++i) {//creates random pool of chromosomes
					pool[i] = Evolvable();
					pool[i].traits.Randormalize();
				}
				CreateDominantPools();
			}

			void GetTraits();

			void SetPerformanceData() {

			}
			//talk to ghost to see stuff
			void SetWaveSize(const size_t wave_size, const float _topPercentage = 0.5f, const float _randPercentage = 0.2f) {
				topPercentage = _topPercentage;
				randPercentage = _randPercentage;
				waveSize = (wave_size);
				//traitPoolSampleSize = (size_t(waveSize * (1.0f / DOMINANT_TRAITS)));
				surviveCount = (size_t(waveSize * topPercentage));
				randomCount = (size_t(waveSize * randPercentage));
				traitPoolSize = (surviveCount + randomCount);

				CreateDominantPools();
			}

		private:
			//template<class Type>
			//static std::unordered_map<Evolver*, Type> templates;

			float topPercentage, randPercentage;

			const size_t generationsToKeep = 2; 

			size_t currentGeneration = 0;//this will need to be loaded from a file

			//not sure if this should be static or instanced
			size_t waveSize;
			size_t surviveCount; //topPercentage%
			size_t randomCount; //randPercentage%
			//size_t traitPoolSampleSize; // 1/DOMINANT_TRAITS
			size_t traitPoolSize;

			DominantPool pool;

			const size_t DOMINANT_TRAITS = 4;
			DominantPool aliveTimePool, damageDeltPool, damageReceivedPool, nodesTraversedPool;

			bool PruneGenerations(const DominantGene & o);

			void PerformFirstSelection(void);
			void PerformFirstMutation(void);

			void CreateDominantPools(void);
			void PerformSelection(void);
			void PerformMutation(void);
			void SelectGenesFromDom(void);
			void Combination(void);

			void CreateTestSamplePerformanceData(void);
		};
	}
}

//template<class Type> std::unordered_map<Omiracon::Genetics::Evolver*, Type> Omiracon::Genetics::Evolver::templates;
//template<typename T> void Omiracon::Genetics::Evolver::Evolve(void) { templates<T>.insert(std::pair<Evolver*, T>(this, T())); }
