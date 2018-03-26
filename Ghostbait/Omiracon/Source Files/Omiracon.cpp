// Omiracon.cpp : Defines the entry point for the console application.
#include "Evolve.h"
#include <iostream>

#include "vld.h"

#include "RandomEngine.h"
using namespace Omiracon::Genetics;

int main() {
	//TraitedEnemy enemy;

	Evolver darwin(1000);
	//darwin.Evolve<TraitedEnemy>();

	for(size_t i = 0; i < 500; i++) {
		std::cout << "Gen: " << i << "\n";
		darwin.RunGeneration();
	}
	darwin.GetBestStats();
	
	return 0;
}