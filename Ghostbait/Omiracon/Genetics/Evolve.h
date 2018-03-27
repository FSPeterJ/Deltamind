#pragma once

#include <vector>
#include "Traits.h"

namespace Omiracon {
	namespace Genetics {
		//class Traits; class Performance;

		class Evolver {
			struct Entity {
				Traits traits;
				Performance measure;
				Entity();
				~Entity();

				//void CreateSimulatedResults(void) {
				//	float timeLasted = Random::RandomNumber(1.0f, 60.0f)*traits[SPEED];
				//	float damageDelt = Random::RandomNumber(0.0f, 100.0f)*traits[STRENGTH];
				//	float damageRecv = Random::RandomNumber(0.0f, 100.0f)*(1.0f - traits[DEFENSE]);
				//	float nodesTrav = Random::RandomNumber(0.0f, 500.0f)*(1.0f - traits[INTELLIGENCE]);
				//
				//	measure.timeLasted = timeLasted + timeLasted * (traits[SPEED] + traits[ENERGY] * 0.5f + traits[COORDINATION] * 0.25f + traits[BALANCE] * 0.125f);
				//	measure.damageDelt = damageDelt + damageDelt * (traits[STRENGTH] + traits[POWER] * 0.5f + traits[ACCURACY] * 0.25f + traits[LUCK] * 0.125f);
				//	measure.damageReceived = damageRecv - damageRecv * (traits[DEFENSE] + traits[ENDURANCE] * 0.5f + traits[STAMINA] * 0.25f + traits[RESISTANCE] * 0.125f);
				//	measure.nodesTraversed = nodesTrav - nodesTrav * (traits[INTELLIGENCE] + traits[WISDOM] * 0.5f + traits[EVASION] * 0.25f + traits[DEXTERITY] * 0.125f);
				//
				//	measure.damageReceived = measure.damageReceived < 0 ? 0 : measure.damageReceived;
				//	measure.nodesTraversed = measure.nodesTraversed < 0 ? 0 : measure.nodesTraversed;
				//}
			};
		public:
		typedef std::vector<Entity> DominantPool;
		private:
			//template<class Type>
			//static std::unordered_map<Evolver*, Type> templates;

			//not sure if this should be static or instanced
			const size_t waveSize;
			const size_t surviveCount; //topPercentage%
			const size_t randomCount; //randPercentage%
			const size_t traitPoolSampleSize; // 1/DOMINANT_TRAITS
			const size_t traitPoolSize;

			DominantPool pool;

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

			//template <class T> void Evolve(void);
		};

	}
}

//template<class Type> std::unordered_map<Omiracon::Genetics::Evolver*, Type> Omiracon::Genetics::Evolver::templates;
//template<typename T> void Omiracon::Genetics::Evolver::Evolve(void) { templates<T>.insert(std::pair<Evolver*, T>(this, T())); }
