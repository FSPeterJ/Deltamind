#pragma once

#include "TraitedEnemy.h"
#include <vector>
namespace Omiracon {
	namespace Genetics {
		typedef std::vector<TraitedEnemy> DominantPool;

		class Evolver {
			const size_t waveSize;
			const size_t surviveCount; //50%
			const size_t randomCount; //20%
			const size_t traitPoolSampleSize; // 1/numDomTraits
			const size_t traitPoolSize;

			std::vector<TraitedEnemy> pool;
			DominantPool aliveTimePool, damageDeltPool, damageReceivedPool, nodesTraversedPool;
			//TraitedEnemy** aliveTimePool, **damageDeltPool, **damageReceivedPool, **nodesTraversedPool;

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
			Evolver(size_t _wave_size);
			~Evolver(void);
		};
	}
}
