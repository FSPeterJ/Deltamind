#include "stdafx.h"
#include "RandomEngine.h"
#include <random>
#include <functional>

static std::random_device r;

std::mt19937 GetRnd() {
	std::seed_seq seed {r(), r(), r(), r(), r(), r(), r(), r()};
	return std::mt19937(seed);
}

double GhostMind::GhostRand::RandomDoubleOpenMax(double lb, double ub) {
	return std::bind(std::uniform_real_distribution<double>(lb, ub), GetRnd())();
}

int GhostMind::GhostRand::RandomIntOpenMax(int lb, int ub) {
	return std::bind(std::uniform_int_distribution<int>(lb, ub), GetRnd())();
}

double GhostMind::GhostRand::RandomDoubleClosedMax(double lb, double ub) {
	return std::bind(std::uniform_real_distribution<double>(lb, std::nextafter(ub, ub + 1.0)), GetRnd())();
}

int GhostMind::GhostRand::RandomIntClosedMax(int lb, int ub) {
	return std::bind(std::uniform_int_distribution<int>(lb, std::nextafter(ub, ub + 1)), GetRnd())();
}
