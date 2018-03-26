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
			friend class Evolver;

			float traitList[Trait::COUNT];
		public:
			Traits(void) {}
			~Traits(void) {}

			void Randomize(void);
			void Normalize(void);
			float Sum(void);

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
			float timeLasted = 0.f,
			damageDelt = 0.f,
			damageReceived = 0.f,
			nodesTraversed = 0.f;

			void Reset(void) { timeLasted = damageDelt = damageReceived = nodesTraversed = 0.f; }
		};
	}
}
