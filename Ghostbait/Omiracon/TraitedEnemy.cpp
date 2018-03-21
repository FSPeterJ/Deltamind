#include "stdafx.h"
#include "TraitedEnemy.h"
#include "RandomEngine.h"

#include <iostream>
#include <iomanip>

#include <vector>

#include<functional>
using Mutation = std::function<void(Traits& traits)>;

//A random gene is selected and its value is changed with a random value between lower and upper bound.
Mutation Creep = [&](Traits& traits) {
	float mutationFactor = 0.1f;
	traits.RandomTrait() += GhostRand::RandomNumber<float, GhostRand::Type::Inclusive>(-mutationFactor, mutationFactor);
};

//Changes the value of chosen gene with uniform random value selected between the user specified upper and lower bound for that gene.
Mutation Uniform = [&](Traits& traits) {
	//right now this is the same as Creep
	float mutationFactor = 0.1f;
	traits.RandomTrait() += GhostRand::RandomNumber<float, GhostRand::Type::Inclusive>(-mutationFactor, mutationFactor);
};

//Two random positions of the string are chosen and the bits corresponding to those positions are interchanged.
Mutation Interchanging = [&](Traits& traits) {
	std::swap(traits.RandomTrait(), traits.RandomTrait());
};

//Pick a subset of genes at random and then randomly rearrange the alleles in those positions.
Mutation Scramble = [&](Traits& traits) {
	std::size_t genesToScramble = GhostRand::RandomNumber(0, static_cast<int>(Trait::COUNT));
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


void Traits::Randomize(void) { 
	for(float *i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; *(i++) = GhostRand::RandomNumber<float, GhostRand::Type::Inclusive>(0.0, 1.0)); 
}
inline float const Traits::Sum(void) { float sum = 0.0f; for(float* i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; sum += *i++); return sum; }

inline float & Traits::RandomTrait(void) { return traitList[GhostRand::RandomNumber(0, static_cast<int>(Trait::COUNT))]; }

inline void Traits::Normalize(void) {
	for(float n = 1.f / Sum(), *i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; *(i++) *= n);
	traitList[BALANCE] += 1.f - Sum();
}

TraitedEnemy::TraitedEnemy(void) {
	float s = traits.Sum();
	
	std::cout << std::setprecision(100) << "sum is now " << s << '\n';
	std::cout << "INTELLIGENCE is now " << traits[INTELLIGENCE] << '\n';
	std::cout << "DEFENSE is now " << traits[DEFENSE]<< '\n';
	std::cout << "STRENGTH is now " << traits[STRENGTH]<< '\n';
	std::cout << "SPEED is now " << traits[SPEED]<< '\n';
	std::cout << "WISDOM is now " << traits[WISDOM]<< '\n';
	std::cout << "ENDURANCE is now " << traits[ENDURANCE]<< '\n';
	std::cout << "POWER is now " << traits[POWER]<< '\n';
	std::cout << "ENERGY is now " << traits[ENERGY]<< '\n';
	std::cout << "EVASION is now " << traits[EVASION]<< '\n';
	std::cout << "STAMINA is now " << traits[STAMINA]<< '\n';
	std::cout << "ACCURACY is now " << traits[ACCURACY]<< '\n';
	std::cout << "COORDINATION is now " << traits[COORDINATION]<< '\n';
	std::cout << "DEXTERITY is now " << traits[DEXTERITY]<< '\n';
	std::cout << "RESISTANCE is now " << traits[RESISTANCE]<< '\n';
	std::cout << "LUCK is now " << traits[LUCK]<< '\n';
	std::cout << "BALANCE is now " << traits[BALANCE]<< '\n';

}

TraitedEnemy::~TraitedEnemy(void) {}
