#include "Traits.h"
#include <functional>
#include <iomanip>
#include <iostream>
#include <vector>
#include "RandomEngine.h"

namespace Omiracon {
	namespace Genetics {
		using Mutation = std::function<void(Traits& traits)>;

		//A random gene is selected and its value is changed with a random value between lower and upper bound.
		Mutation Creep = [&](Traits& traits) {
			float mutationFactor = 0.1f;
			float& chosenTrait = traits.RandomTrait();
			chosenTrait += Random::RandomNumber(-mutationFactor, mutationFactor);
			if(chosenTrait < 0.0f) chosenTrait = 0.0f;
		};

		//Changes the value of chosen gene with uniform random value selected between the user specified upper and lower bound for that gene.
		Mutation Uniform = [&](Traits& traits) {
			//right now this is the same as Creep
			float mutationFactor = 0.1f;
			traits.RandomTrait() += Random::RandomNumber(-mutationFactor, mutationFactor);
		};

		//Two random positions of the string are chosen and the bits corresponding to those positions are interchanged.
		Mutation Interchanging = [&](Traits& traits) {
			std::swap(traits.RandomTrait(), traits.RandomTrait());
		};

		//Pick a subset of genes at random and then randomly rearrange the alleles in those positions.
		Mutation Scramble = [&](Traits& traits) {
			std::size_t genesToScramble = Random::RandomNumber(0, static_cast<int>(Trait::COUNT));
			std::vector<float> geneSubset(genesToScramble);
			for(std::size_t i = 0; i < genesToScramble; ++i) {
				geneSubset[i] = traits.RandomTrait();
			}
		};

		void Traits::Mutate(const MutationType mutation) {
			switch(mutation) {
				//case INSERT: break;
				//case INVERSION: break;
			case SCRAMBLE: Scramble(*this); break;
				//case SWAP: break;
				//case FLIP: break;
			case INTERCHANGING: Interchanging(*this); break;
				//case REVERSING: break;
			case UNIFORM: Uniform(*this); break;
			default:case CREEP: Creep(*this); break;
			}
			Normalize();
		}

		void Traits::Reset(void) {
			memset(traitList, 0x0, Trait::COUNT * sizeof(float));
		}

		void Traits::Randomize(void) {
			for(float *i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; *(i++) = Random::RandomNumber(0.0f, 1.0f));
		}

		inline float Traits::Sum(void) { float sum = 0.0f; for(float* i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; sum += *i++); return sum; }

		inline float& Traits::RandomTrait(void) { return traitList[Random::RandomNumber(0, static_cast<int>(Trait::COUNT))]; }

		inline void Traits::Normalize(void) {
			for(float n = 1.f / Sum(), *i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; *(i++) *= n);
			traitList[BALANCE] += 1.f - Sum();
		}

#define getPretty(f) (f*100.0f)
#define NL  std::endl <<  std::endl
		void Traits::Print(void) const {
			std::cout << std::setprecision(100);
			std::cout << "INTELLIGENCE is now " << getPretty(operator[](INTELLIGENCE)) << '\n';
			std::cout << "WISDOM is now " << getPretty(operator[](WISDOM)) << '\n';
			std::cout << "EVASION is now " << getPretty(operator[](EVASION)) << '\n';
			std::cout << "DEXTERITY is now " << getPretty(operator[](DEXTERITY)) << NL;

			std::cout << "\nDEFENSE is now " << getPretty(operator[](DEFENSE)) << '\n';
			std::cout << "ENDURANCE is now " << getPretty(operator[](ENDURANCE)) << '\n';
			std::cout << "STAMINA is now " << getPretty(operator[](STAMINA)) << '\n';
			std::cout << "RESISTANCE is now " << getPretty(operator[](RESISTANCE)) << NL;

			std::cout << "\nSTRENGTH is now " << getPretty(operator[](STRENGTH)) << '\n';
			std::cout << "POWER is now " << getPretty(operator[](POWER)) << '\n';
			std::cout << "ACCURACY is now " << getPretty(operator[](ACCURACY)) << '\n';
			std::cout << "LUCK is now " << getPretty(operator[](LUCK)) << NL;

			std::cout << "\nSPEED is now " << getPretty(operator[](SPEED)) << '\n';
			std::cout << "ENERGY is now " << getPretty(operator[](ENERGY)) << '\n';
			std::cout << "COORDINATION is now " << getPretty(operator[](COORDINATION)) << '\n';
			std::cout << "BALANCE is now " << getPretty(operator[](BALANCE)) << NL;
		}

		void Performance::Measure::Reset(void) {
			memset(this, 0x0, sizeof(this));
		}
	}
}
