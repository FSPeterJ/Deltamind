#pragma once

namespace Omiracon {
	namespace Random {
		enum class Type {
			Inclusive,
			Exclusive,
		};

		template<typename T = int, Type Interval = (std::is_integral<T>::value ? Type::Exclusive : Type::Inclusive)>
		T RandomNumber(const T min, const T max);
	}
}
