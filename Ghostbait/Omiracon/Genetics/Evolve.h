#pragma once

#include <vector>
#include "Evolvable.h"

namespace Omiracon {
	namespace Genetics {
		class Evolver {
		public:
			typedef std::vector<Evolvable> DominantPool;
			typedef DominantPool::value_type DominantGene;

			/// <summary>
			/// Gets the gene pool to sample from.
			/// </summary>
			/// <returns>Omiracon.Genetics.Evolvable * const.</returns>
			Evolvable *const GenePool(void) { return &genepool[0]; }

			Evolver(void);
			~Evolver(void);

			/// <summary>
			/// After a wave is completed, this processes the generation's data and prepares the next genes.
			/// </summary>
			void RunGeneration(void);

			/// <summary>
			/// Prints the best stats for each dominant trait.
			/// </summary>
			void PrintBestStats(void);

			/// <summary>
			/// Used to initalize the information pertaining to the first wave.
			/// </summary>
			/// <param name="wave_size">Size of the wave.</param>
			/// <param name="_topPercentage">The percentage of entities to keep as the best.</param>
			/// <param name="_randPercentage">The percentage of random entities to choose.</param>
			void SetFirstWave(const size_t wave_size, const float _topPercentage = 0.5f, const float _randPercentage = 0.2f) {
				topPercentage = _topPercentage;
				randPercentage = _randPercentage;
				waveSize = (wave_size);
				//traitPoolSampleSize = (size_t(wave_size * (1.0f / DOMINANT_TRAITS)));
				surviveCount = (size_t(waveSize * topPercentage));
				randomCount = (size_t(waveSize * randPercentage));
				traitPoolSize = 2 * (surviveCount + randomCount);

				genepool.resize(traitPoolSize * DOMINANT_TRAITS);

				testpool.resize(waveSize);

				for(size_t i = 0; i < waveSize; ++i) {//creates random pool of chromosomes
					genepool[i] = Evolvable();
					genepool[i].traits.Randormalize();
				}

				genDeathSize = (surviveCount + randomCount) * DOMINANT_TRAITS;
				nextGenDeathSize = genDeathSize;

				CreateDominantPools();
			}

			/// <summary>
			/// Used to initalize the information pertaining to the next wave.
			/// </summary>
			/// <param name="wave_size">Size of the wave.</param>
			/// <param name="_topPercentage">The top percentage.</param>
			/// <param name="_randPercentage">The rand percentage.</param>
			void SetWaveSize(const size_t wave_size, const float _topPercentage = 0.5f, const float _randPercentage = 0.2f) {
				topPercentage = _topPercentage;
				randPercentage = _randPercentage;
				waveSize = (wave_size);
				//traitPoolSampleSize = (size_t(waveSize * (1.0f / DOMINANT_TRAITS)));
				surviveCount = (size_t(waveSize * topPercentage));
				randomCount = (size_t(waveSize * randPercentage));
				traitPoolSize = (surviveCount + randomCount);

				testpool.resize(waveSize);

				genepool.resize((genepool.size() /*- genDeathSize*/) + (traitPoolSize * DOMINANT_TRAITS));
				//previousSize = genepool.size();

				CreateDominantPools();

				genDeathSize = nextGenDeathSize;

				nextGenDeathSize = traitPoolSize * DOMINANT_TRAITS;
			}
		private:
			const size_t DOMINANT_TRAITS = 4;
			const size_t generationsToKeep = 2;

			float topPercentage, randPercentage;

			int currentGeneration = 0;//this will need to be loaded from a file. if restart happens, the pool is cleared and messes things up

			size_t
				waveSize, //size of current wave
				surviveCount, //number of entities that survived via best genes
				randomCount, //number of entities that survived via random chance
				traitPoolSize, //size of current trait pool
				previousSize = 0, //!size of previous wave
				genDeathSize = 0, //number of entities that died off due to old age
				nextGenDeathSize = 0; //number of entities that will die off next generation

			DominantPool genepool, testpool, aliveTimePool, damageDeltPool, damageReceivedPool, nodesTraversedPool;

			inline size_t GetMemAddr(const size_t index) const { return index * sizeof(DominantGene); }

			void CreateDominantPools(void);

			void PerformFirstSelection(void);
			void PerformSelection(void);

			void PerformFirstMutation(void);
			void PerformMutation(void);

			void Combination(void);
		};
	}
}

//template<class Type> std::unordered_map<Omiracon::Genetics::Evolver*, Type> Omiracon::Genetics::Evolver::templates;
//template<typename T> void Omiracon::Genetics::Evolver::Evolve(void) { templates<T>.insert(std::pair<Evolver*, T>(this, T())); }
