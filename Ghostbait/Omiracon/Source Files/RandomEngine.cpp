#include "RandomEngine.h"
#include <random>
#include <functional>
#include <algorithm>

static std::random_device rd;
static std::seed_seq seed {rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
static std::mt19937 rnd(seed);

template<typename T, Omiracon::Random::Type Interval>
T Omiracon::Random::RandomNumber(const T lb, const T ub) {
	using udist = typename std::conditional<std::is_integral<T>::value,std::uniform_int_distribution<T>,std::uniform_real_distribution<T>>::type;
	using flt = std::is_floating_point<T>;

	const bool closed = Interval == Type::Inclusive, isFlt = flt();

	return (!closed && isFlt || closed && !isFlt) ? udist(lb, ub)(rnd) : closed ? udist(lb, T(std::nextafter(ub, ub + 1.0)))(rnd) : udist(lb, ub - 1)(rnd);

	//readable version:
	//switch(Interval) {
	//case Type::Inclusive:
	//	if(std::is_floating_point<T>()) {
	//		return std::bind(uniform_distribution(lb, T(std::nextafter(ub, ub + 1.0))), GetRnd())();
	//	}
	//	return std::bind(uniform_distribution(lb, ub), GetRnd())();
	//default: case Type::Exclusive:
	//	if(std::is_floating_point<T>()) {
	//		return std::bind(uniform_distribution(lb, ub), GetRnd())();
	//	}
	//	return std::bind(uniform_distribution(lb, ub - 1), GetRnd())();
	//}
}

template int Omiracon::Random::RandomNumber<int, Omiracon::Random::Type::Inclusive>(const int min, const int max);
template double Omiracon::Random::RandomNumber<double, Omiracon::Random::Type::Inclusive>(const double min, const double max);
template float Omiracon::Random::RandomNumber<float, Omiracon::Random::Type::Inclusive>(const float min, const float max);

template int Omiracon::Random::RandomNumber<int, Omiracon::Random::Type::Exclusive>(const int min, const int max);
template double Omiracon::Random::RandomNumber<double, Omiracon::Random::Type::Exclusive>(const double min, const double max);
template float Omiracon::Random::RandomNumber<float, Omiracon::Random::Type::Exclusive>(const float min, const float max);
