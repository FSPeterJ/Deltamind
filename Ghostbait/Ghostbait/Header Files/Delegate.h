#pragma once
#include <functional>
#include <vector>


#define estimated_objects 10

template <typename ...T>
class Delegate: std::function<void(T...)> {
	std::vector<std::function<void(T...)>> delegates;
public:
	void operator( )(const T&... e) const {
		for(const std::function<void(T...)> element : delegates) {
			element(e...);
		}
	}

	const inline size_t subscriber_count() const { return delegates.size(); }
	inline void add(const std::function<void(T...)> execute) { delegates.push_back(execute); }
	inline void insert(const std::function<void(T...)> execute, size_t index) { delegates.insert(delegates.begin() + index, execute); }

	Delegate(void) { delegates.reserve(estimated_objects); };
	~Delegate(void) {};
};
