#pragma once

#include "TraitedEnemy.h"
#include <vector>
namespace Omiracon {
	namespace Genetics {
		typedef std::vector<TraitedEnemy> DominantPool;

		class Evolver {
			const size_t waveSize;
			const size_t surviveCount; //topPercentage%
			const size_t randomCount; //randPercentage%
			const size_t traitPoolSampleSize; // 1/DOMINANT_TRAITS
			const size_t traitPoolSize;

			std::vector<TraitedEnemy> pool;

			const size_t DOMINANT_TRAITS = 4;
			DominantPool aliveTimePool, damageDeltPool, damageReceivedPool, nodesTraversedPool;

			void CreateDominantPools(void);
			void FillDominantPools(void);
			void MutateDominantPools(void);
			void SelectGenesFromDominantPools(void);
			void ConstructPoolWithMutatedGenes(void);

			void CreateTestSamplePerformanceData(void);
		public:
			void RunGeneration(void);
			void GetBestStats(void);
			//void Evolve(size_t generations);
			Evolver(size_t _wave_size, float topPercentage = 0.5f, float randPercentage = 0.2f);
			~Evolver(void);
		};
	}
}
