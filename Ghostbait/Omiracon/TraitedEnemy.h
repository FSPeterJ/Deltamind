#pragma once

enum Trait {
	INTELLIGENCE, DEFENSE, STRENGTH, SPEED,
	WISDOM, ENDURANCE, POWER, ENERGY,
	EVASION, STAMINA, ACCURACY, COORDINATION,
	DEXTERITY, RESISTANCE, LUCK, BALANCE,
	COUNT
};

class Traits {
	friend class TraitedEnemy;

	float* traitList = nullptr;

	Traits(void) { traitList = new float[Trait::COUNT] , Randomize(), Normalize(); }
	~Traits(void) { delete[] traitList; }
public:
	void Randomize(void);
	void Normalize(void);
	float const Sum(void);

	void Reset(void) { memset(traitList, 0x0, Trait::COUNT * sizeof(float)); }
	float const& operator[](const Trait trait) const { return traitList[static_cast<int>(trait)]; }
	float& operator[](const Trait trait) { return traitList[static_cast<int>(trait)]; }
};

class Performance {

public:
	float timeLasted;
	float damageDelt;
	float damageReceived;
	int nodesTraversed;
};

class TraitedEnemy {
	Traits traits;
public:
	Performance measure;
	TraitedEnemy();
	~TraitedEnemy();
};
