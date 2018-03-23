#pragma once

#include "RandomEngine.h"
namespace Omiracon {
	namespace Genetics {

		enum Trait {
			INTELLIGENCE, DEFENSE, STRENGTH, SPEED,
			WISDOM, ENDURANCE, POWER, ENERGY,
			EVASION, STAMINA, ACCURACY, COORDINATION,
			DEXTERITY, RESISTANCE, LUCK, BALANCE,
			COUNT
		};

		enum MutationType {
			//INSERT,
			//INVERSION,
			SCRAMBLE,
			//SWAP,
			//FLIP,
			INTERCHANGING,
			//REVERSING,
			UNIFORM,
			CREEP
		};

		class Traits {
			friend class TraitedEnemy;

			float traitList[Trait::COUNT];

			Traits(void) {
				//traitList = new float[Trait::COUNT];
			}
			~Traits(void) {
				//	delete[] traitList;
			}
		public:
			void Randomize(void);
			void Normalize(void);
			float const Sum(void);

			void Print(void);

			void Randormalize(void) { Randomize(), Normalize(); }
			void Reset(void) { memset(traitList, 0x0, Trait::COUNT * sizeof(float)); }
			float const& operator[](const Trait trait) const { return traitList[static_cast<int>(trait)]; }
			float& operator[](const Trait trait) { return traitList[static_cast<int>(trait)]; }

			float const& operator[](const int n) const { return traitList[n]; }


			inline float& RandomTrait(void);
			void Mutate(const MutationType mutation);
		};

		class Performance {
		public:
			float timeLasted = 0;
			float damageDelt = 0;
			float damageReceived = 0;
			float nodesTraversed = 0;
			void Reset(void) {
				timeLasted = 0;
				damageDelt = 0;
				damageReceived = 0;
				nodesTraversed = 0;
			}
		};

		class TraitedEnemy {
		public:
			Performance measure;
			Traits traits;

			void CreateSimulatedResults(void) {
				float timeLasted = Random::RandomNumber<float, Random::Type::Inclusive>(1.0f, 60.0f)*traits[SPEED];
				float damageDelt = Random::RandomNumber<float, Random::Type::Inclusive>(0.0f, 100.0f)*traits[STRENGTH];
				float damageRecv = Random::RandomNumber<float, Random::Type::Inclusive>(0.0f, 100.0f)*(1.0f - traits[DEFENSE]);
				float nodesTrav = Random::RandomNumber<float, Random::Type::Inclusive>(0.0f, 500.0f)*(1.0f - traits[INTELLIGENCE]);

				measure.timeLasted = timeLasted + timeLasted * (traits[SPEED] + traits[ENERGY] * 0.5f + traits[COORDINATION] * 0.25f + traits[BALANCE] * 0.125f);
				measure.damageDelt = damageDelt + damageDelt * (traits[STRENGTH] + traits[POWER] * 0.5f + traits[ACCURACY] * 0.25f + traits[LUCK] * 0.125f);
				measure.damageReceived = damageRecv - damageRecv * (traits[DEFENSE] + traits[ENDURANCE] * 0.5f + traits[STAMINA] * 0.25f + traits[RESISTANCE] * 0.125f);
				measure.nodesTraversed = nodesTrav - nodesTrav * (traits[INTELLIGENCE] + traits[WISDOM] * 0.5f + traits[EVASION] * 0.25f + traits[DEXTERITY] * 0.125f);

				measure.damageReceived = measure.damageReceived < 0 ? 0 : measure.damageReceived;
				measure.nodesTraversed = measure.nodesTraversed < 0 ? 0 : measure.nodesTraversed;
			}

			TraitedEnemy(void);
			~TraitedEnemy(void);
		};
	}
}
