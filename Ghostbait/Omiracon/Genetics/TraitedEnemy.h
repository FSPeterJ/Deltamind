#pragma once

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

			Traits(void) {}
			~Traits(void) {}
		public:
			void Randomize(void);
			void Normalize(void);
			float const Sum(void);

			void Print(void) const;

			void Randormalize(void) { Randomize(), Normalize(); }
			void Reset(void);

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
				timeLasted = damageDelt = damageReceived = nodesTraversed = 0.0f;
			}
		};

		class TraitedEnemy {
		public:
			Performance measure;
			Traits traits;

			void CreateSimulatedResults(void);

			TraitedEnemy(void);
			~TraitedEnemy(void);
		};
	}
}
