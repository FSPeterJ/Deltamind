#pragma once
#include <functional>
#include <vector>

template <typename ...T>
class Delegate: std::function<void(T...)> {
	 std::vector<std::function<void(T...)>> delegates;
public:
	void operator()(const T&... e) const {
		for(const std::function<void(T...)> element : delegates) {
			element(e...);
		}
	}

	inline void add(const std::function<void(T...)> execute) { delegates.push_back(execute); }

	Delegate(void) {};
	~Delegate(void) {};
};

