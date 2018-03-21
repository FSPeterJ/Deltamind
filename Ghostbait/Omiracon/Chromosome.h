#pragma once
struct Gene;
class Chromosome {
	double fitness;
	Gene* genes;
public:
	Chromosome();
	~Chromosome();
};

