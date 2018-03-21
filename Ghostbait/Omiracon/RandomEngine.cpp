#include "stdafx.h"
#include "RandomEngine.h"
#include <random>
#include <functional>

static std::random_device r;

std::mt19937 GetRnd() {
	std::seed_seq seed {r(), r(), r(), r(), r(), r(), r(), r()};
	return std::mt19937(seed);
}

template<typename T, GhostRand::Type Interval>
T GhostRand::RandomNumber(T lb, T ub) {
	using uniform_distribution =
		typename std::conditional<
		std::is_integral<T>::value,
		std::uniform_int_distribution<T>,
		std::uniform_real_distribution<T>
		>::type;
	switch(Interval) {
	case Type::Inclusive:
		if(std::is_floating_point<T>()) {
			return std::bind(uniform_distribution(lb, std::nextafter(ub, ub + 1.0)), GetRnd())();
		}
		return std::bind(uniform_distribution(lb, ub), GetRnd())();
	case Type::Exclusive:
		if(std::is_floating_point<T>()) {
			return std::bind(uniform_distribution(lb, ub), GetRnd())();
		}
		return std::bind(uniform_distribution(lb, ub - 1), GetRnd())();
	}
}

template int GhostRand::RandomNumber<int, GhostRand::Type::Inclusive>(int min, int max);
template double GhostRand::RandomNumber<double,GhostRand::Type::Inclusive>(double min, double max);
template float GhostRand::RandomNumber<float, GhostRand::Type::Inclusive>(float min, float max);

template int GhostRand::RandomNumber<int, GhostRand::Type::Exclusive>(int min, int max);
template double GhostRand::RandomNumber<double, GhostRand::Type::Exclusive>(double min, double max);
template float GhostRand::RandomNumber<float, GhostRand::Type::Exclusive>(float min, float max);
