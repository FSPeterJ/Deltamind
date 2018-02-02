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
	size_t delegate_iteration = 0;
public:
	void operator()(const T&... e) {
		size_t delsize = delegates.size();
		//Iterators were not cutting the cake for us.  Accessing by index is ok, and we prevet iterator invalidation by pushback mid iteration
		for(delegate_iteration = 0; delegate_iteration < delsize; delegate_iteration++) {
			delegates[delegate_iteration].function(e...);
		}
		//Process the delegate further if more things were added to the current delegate by previous delegate calls in this update
		if(delsize < delegates.size()) {
			for(delegate_iteration = delsize; delegate_iteration < delegates.size(); delegate_iteration++) {
				delegates[delegate_iteration].function(e...);
			}
		}
		delegate_iteration = 0;
	}
	//Possibly want to phase this out.  Unsure what problems we will run into with legacy support and 0 being an ID for permament registration
	//ONLY use this if you NEVER want to remove your delegated function post-registration
	void operator+=(const std::function<void(T...)> execute) {
		Delegate_Entry data = {0, execute};
		delegates.push_back(data);
	}

	unsigned Add(const std::function<void(T...)> execute) {
		Delegate_Entry data = {++lastID, execute};
		delegates.push_back(data);
		return data.id;
	}

	void Remove(unsigned id) {
		//Bad ID given.  You cannot remove permament delegates as all permament delegates are give the same ID of 0
		assert(id != 0);

		//Error: You cannot call remove on a delegate from within that same delegate process loop!
		//(you cannot remove a vector element while iterating that vector)
		//Example: While processing all update functions in the Update delegate, you remove thisobject.Update from the Update delegate.
		//Please use a deffered removal method
		assert(delegate_iteration == 0);

		Delegate_Entry data = {id, nullptr};
		std::vector<Delegate_Entry>::iterator it = find(delegates.begin(), delegates.end(), data);
		*it = std::move(delegates.back());
		delegates.pop_back();
	}

	const inline size_t subscriber_count() const { return delegates.size(); }

	inline void insert(const std::function<void(T...)> execute, size_t index) { delegates.insert(delegates.begin() + index, execute); }

	Delegate(void) {};
	~Delegate(void) {};
};
