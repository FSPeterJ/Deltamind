#pragma once
#include <vector>
#include <functional>
#include <algorithm>


template <typename ...T>
class Delegate: std::function<void(T...)> {

	struct Delegate_Entry {
		unsigned id;
		std::function<void(T...)> function;
		//----------
		// Operators 
		//===========================
		bool operator ==(const Delegate_Entry &B) const {
			return id == B.id;
		}
		//possibly change this to execute function, but may be unessesary
		bool operator () (const Delegate_Entry& B) const {
			return id == B.id;
		}

	};

	//vld HATES this resizing itself but less than 1 second at runtime without vld, non-issue for now
	std::vector<Delegate_Entry> delegates;
	unsigned lastID = 0;
public:
	void operator()(const T&... e) const {

		for(const Delegate_Entry &element : delegates) {
			element.function(e...);
		}
	}
	//Possibly want to phase this out.  Unsure what problems we will run into with 0 being an ID for permament registration
	void operator+=(const std::function<void(T...)> execute) {
		Delegate_Entry data = { 0, execute };
		delegates.push_back(data);
	}

	unsigned Add(const std::function<void(T...)> execute) {
		Delegate_Entry data = { ++lastID, execute };
		delegates.push_back(data);
		return data.id;
	}

	void Remove(unsigned id) {
		if(id == 0)
		{
			throw "Bad ID.  You cannot remove permament delegates (ID 0)";
		}
		Delegate_Entry data = { id, nullptr };
		std::vector<Delegate_Entry>::iterator it = find(delegates.begin(), delegates.end(), data);
		*it = std::move(delegates.back());
		delegates.pop_back();
	}

	const inline size_t subscriber_count() const { return delegates.size(); }

	inline void insert(const std::function<void(T...)> execute, size_t index) { delegates.insert(delegates.begin() + index, execute); }

	Delegate(void) {};
	~Delegate(void) {};
};
