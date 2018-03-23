#include "stdafx.h"
#include "RandomEngine.h"
#include <random>
#include <functional>
#include <algorithm>

static std::random_device r;

std::mt19937 GetRnd() {
	std::seed_seq seed {r(), r(), r(), r(), r(), r(), r(), r()};
	return std::mt19937(seed);
}

template<typename T, Omiracon::Random::Type Interval>
T const Omiracon::Random::RandomNumber(const T lb, const T ub) {
	using uniform_distribution = typename std::conditional<std::is_integral<T>::value,std::uniform_int_distribution<T>,std::uniform_real_distribution<T>>::type;
	switch(Interval) {
	case Type::Inclusive:
		if(std::is_floating_point<T>()) {
			return std::bind(uniform_distribution(lb, T(std::nextafter(ub, ub + 1.0))), GetRnd())();
		}
		return std::bind(uniform_distribution(lb, ub), GetRnd())();
	default: case Type::Exclusive:
		if(std::is_floating_point<T>()) {
			return std::bind(uniform_distribution(lb, ub), GetRnd())();
		}
		return std::bind(uniform_distribution(lb, ub - 1), GetRnd())();
	}
}

template<typename T>
void Omiracon::Random::ShuffleArray(T* arr, size_t size) {
	std::random_shuffle(arr, arr+size, GetRnd());
}

template int const Omiracon::Random::RandomNumber<int, Omiracon::Random::Type::Inclusive>(const int min, const int max);
template double const Omiracon::Random::RandomNumber<double, Omiracon::Random::Type::Inclusive>(const double min, const double max);
template float const Omiracon::Random::RandomNumber<float, Omiracon::Random::Type::Inclusive>(const float min, const float max);

template int const Omiracon::Random::RandomNumber<int, Omiracon::Random::Type::Exclusive>(const int min, const int max);
template double const Omiracon::Random::RandomNumber<double, Omiracon::Random::Type::Exclusive>(const double min, const double max);
template float const Omiracon::Random::RandomNumber<float, Omiracon::Random::Type::Exclusive>(const float min, const float max);
