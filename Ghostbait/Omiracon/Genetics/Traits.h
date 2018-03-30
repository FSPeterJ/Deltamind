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

			/// <summary>
			/// Randomizes all traits.
			/// </summary>
			void Randomize(void);

			/// <summary>
			/// Normalizes traits.
			/// </summary>
			void Normalize(void);

			/// <summary>
			/// Calculates the sum of all the traits.
			/// </summary>
			/// <returns>float.</returns>
			float Sum(void);

			void Print(void) const;

			/// <summary>
			/// Randomizes all traits and then normalizes.
			/// </summary>
			void Randormalize(void) { Randomize(), Normalize(); }

			/// <summary>
			/// Resets all traits to 0.
			/// </summary>
			void Reset(void);

			float const& operator[](const Trait trait) const { return traitList[static_cast<int>(trait)]; }
			float& operator[](const Trait trait) { return traitList[static_cast<int>(trait)]; }

			float const& operator[](const int n) const { return traitList[n]; }

			const float* const GetTraitArray() const { return traitList; }

			/// <summary>
			/// Gets a random trait of the enemy.
			/// </summary>
			/// <returns>float &.</returns>
			inline float& RandomTrait(void);

			/// <summary>
			/// Mutates the traits via the specified mutation.
			/// </summary>
			/// <param name="mutation">The mutation.</param>
			void Mutate(const MutationType mutation);
		};

		class Performance {
		public:
			union Measure {
				struct { float timeLasted, damageDelt, damageReceived, nodesTraversed; };
				float measure[4];
				void Reset(void);
			} results;

			bool died = false;

			void Reset(void) { results.Reset(), died = 0; }

			Performance(void) { Reset(); }
			~Performance(void) { }
		};
	}
}

