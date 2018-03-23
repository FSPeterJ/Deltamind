#pragma once

namespace Omiracon {
	namespace Random {
		enum class Type {
			Inclusive,
			Exclusive,
		};
		template<typename T = int, Type Interval = Type::Exclusive>
		T const RandomNumber(const T min, const T max);

		template<typename T>
		void ShuffleArray(T* arr, size_t size);
	}
}
