#pragma once

namespace GhostRand {
	enum class Type {
		Inclusive,
		Exclusive,
	};
	template<typename T = int, Type Interval = Type::Exclusive>
	T RandomNumber(T min, T max);
	
}


