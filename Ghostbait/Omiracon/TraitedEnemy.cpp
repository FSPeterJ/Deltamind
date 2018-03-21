#include "stdafx.h"
#include "TraitedEnemy.h"
#include "RandomEngine.h"

#include <iostream>
#include <iomanip>

void Traits::Randomize(void) { for(float *i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; *(i++) = GhostRand::RandomNumber<float, GhostRand::Type::Inclusive>(0.0, 1.0)); }
inline float const Traits::Sum(void) { float sum = 0.0f; for(float* i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; sum += *i++); return sum; }
inline void Traits::Normalize(void) {
	
	for(float n = 1.f / Sum(), *i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end; *(i++) *= n);

	//for(float*i = &(traitList[0]), *end = &(traitList[Trait::COUNT]); i < end;*(i++) ) {
	//	*(i) = std::roundf(*(i)*1000.f);
	//}

}

TraitedEnemy::TraitedEnemy() {
	//float s = traits.Sum();
	//
	//std::cout << std::setprecision(100) << "sum is now " << s << '\n';
	//std::cout << "INTELLIGENCE is now " << traits[INTELLIGENCE] << '\n';
	//std::cout << "DEFENSE is now " << traits[DEFENSE]<< '\n';
	//std::cout << "STRENGTH is now " << traits[STRENGTH]<< '\n';
	//std::cout << "SPEED is now " << traits[SPEED]<< '\n';
	//std::cout << "WISDOM is now " << traits[WISDOM]<< '\n';
	//std::cout << "ENDURANCE is now " << traits[ENDURANCE]<< '\n';
	//std::cout << "POWER is now " << traits[POWER]<< '\n';
	//std::cout << "ENERGY is now " << traits[ENERGY]<< '\n';
	//std::cout << "EVASION is now " << traits[EVASION]<< '\n';
	//std::cout << "STAMINA is now " << traits[STAMINA]<< '\n';
	//std::cout << "ACCURACY is now " << traits[ACCURACY]<< '\n';
	//std::cout << "COORDINATION is now " << traits[COORDINATION]<< '\n';
	//std::cout << "DEXTERITY is now " << traits[DEXTERITY]<< '\n';
	//std::cout << "RESISTANCE is now " << traits[RESISTANCE]<< '\n';
	//std::cout << "LUCK is now " << traits[LUCK]<< '\n';
	//std::cout << "BALANCE is now " << traits[BALANCE]<< '\n';

}


TraitedEnemy::~TraitedEnemy() {}
