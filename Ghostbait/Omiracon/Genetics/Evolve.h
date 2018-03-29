#pragma once

#include <vector>
#include "Evolvable.h"

namespace Omiracon {
	namespace Genetics {
		class Evolver {
			inline size_t GetMemAddr(size_t index) { return index * sizeof(DominantGene); }

		public:
			typedef std::vector<Evolvable> DominantPool;
			typedef DominantPool::value_type DominantGene;

			Evolvable*const GenePool() { return &genepool[0]; }

			void RunGeneration(void);
			void GetBestStats(void);
			//void Evolve(size_t generations);
			Evolver( );

			~Evolver(void);

			//template <class T> void Evolve(void);

			void SetFirstWave(const size_t wave_size, const float _topPercentage = 0.5f, const float _randPercentage = 0.2f) {
				topPercentage = _topPercentage;
				randPercentage = _randPercentage;
				waveSize = (wave_size);
				//traitPoolSampleSize = (size_t(wave_size * (1.0f / DOMINANT_TRAITS)));
				surviveCount = (size_t(waveSize * topPercentage));
				randomCount = (size_t(waveSize * randPercentage));
				traitPoolSize = 2*(surviveCount + randomCount);

				genepool.resize(traitPoolSize * DOMINANT_TRAITS);

				testpool.resize(waveSize);

				for(size_t i = 0; i < waveSize; ++i) {//creates random pool of chromosomes
					genepool[i] = Evolvable();
					genepool[i].traits.Randormalize();
				}

				CreateDominantPools();
			}

			void GetTraits();

			void SetPerformanceData() {

			}

			size_t previousSize = 0;
			//talk to ghost to see stuff
			void SetWaveSize(const size_t wave_size, const float _topPercentage = 0.5f, const float _randPercentage = 0.2f) {
				topPercentage = _topPercentage;
				randPercentage = _randPercentage;
				waveSize = (wave_size);
				//traitPoolSampleSize = (size_t(waveSize * (1.0f / DOMINANT_TRAITS)));
				surviveCount = (size_t(waveSize * topPercentage));
				randomCount = (size_t(waveSize * randPercentage));
				traitPoolSize = (surviveCount + randomCount);

				testpool.resize(waveSize);

				genepool.resize(genepool.size() + traitPoolSize * DOMINANT_TRAITS);
				previousSize = genepool.size();

				CreateDominantPools();
			}

		private:
			//template<class Type>
			//static std::unordered_map<Evolver*, Type> templates;

			float topPercentage, randPercentage;

			const size_t generationsToKeep = 2; 

			int currentGeneration = 0;//this will need to be loaded from a file
			//if restart happens, the pool is cleared and messes things up

			//not sure if this should be static or instanced
			size_t waveSize;
			size_t surviveCount; //topPercentage%
			size_t randomCount; //randPercentage%
			//size_t traitPoolSampleSize; // 1/DOMINANT_TRAITS
			size_t traitPoolSize;

			DominantPool genepool;
			DominantPool testpool;

			const size_t DOMINANT_TRAITS = 4;
			DominantPool aliveTimePool, damageDeltPool, damageReceivedPool, nodesTraversedPool;

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
