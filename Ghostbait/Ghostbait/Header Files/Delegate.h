#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <cassert>
#include "ThreadPool.h"
using namespace Threadding;

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
		for(delegate_iteration = 0; delegate_iteration < delsize; ++delegate_iteration) {
			auto f = delegates[delegate_iteration].function;
			if(f) f(e...);
		}
		//Process the delegate further if more things were added to the current delegate by previous delegate calls in this update
		if(delsize < delegates.size()) {
			for(delegate_iteration = delsize; delegate_iteration < delegates.size(); ++delegate_iteration) {
				auto f = delegates[delegate_iteration].function;
				if (f) f(e...);
			}
		}
		delegate_iteration = 0;
	}

	void RunAsync(const T&... e) {
		size_t delsize = delegates.size();
		//Iterators were not cutting the cake for us.  Accessing by index is ok, and we prevet iterator invalidation by pushback mid iteration
		//ThreadPool::MakeJob([&] {
			for(delegate_iteration = 0; delegate_iteration < delegates.size(); ++delegate_iteration) {
				auto fuc = std::bind(delegates[delegate_iteration].function, e...);
				std::async(std::launch::async, fuc);
			}
	//	});
		//Process the delegate further if more things were added to the current delegate by previous delegate calls in this update
		if(delsize < delegates.size()) {
			for(delegate_iteration = delsize; delegate_iteration < delegates.size(); ++delegate_iteration) {
				auto fuc = std::bind(delegates[delegate_iteration].function, e...);
				std::async(std::launch::async, fuc);
			}
		}
		delegate_iteration = 0;
	}

	//Possibly want to phase this out.  Unsure what problems we will run into with legacy support and 0 being an ID for permament registration
	//ONLY use this if you NEVER want to remove your delegated function post-registration
	void operator+=(const std::function<void(T...)> execute) {
		if (!execute) return;
		Delegate_Entry data = {0, execute};
		delegates.push_back(data);
		assert(delegates.size() < 10000);

	}

	unsigned Add(const std::function<void(T...)> execute) {
		if (!execute) std::runtime_error("Tried to add empty function.");
		Delegate_Entry data = {++lastID, execute};
		delegates.push_back(data);
		assert(delegates.size() < 10000);

		return data.id;
	}

	void Clear() {
		delegates.clear();
	}

	void Remove(unsigned id) {
		// Bad ID given.  You either did not store the correct id for the caller's delegate or you attempted to remove a permament delegate
		// You cannot remove permament delegates (id of 0) as all permament delegates are give the same ID of 0
		assert(id != 0);

		//Error: You cannot call remove on a delegate from within that same delegate process loop!
		//(you cannot remove a vector element while iterating that vector)
		//Example: While processing all update functions in the Update delegate, you try to remove thisobject.Update from the Update delegate.
		//Please use a deferred removal method
		assert(delegate_iteration == 0);

		Delegate_Entry data = {id, nullptr};
		std::vector<Delegate_Entry>::iterator it = find(delegates.begin(), delegates.end(), data);
		if (it != delegates.end()) {
			*it = std::move(delegates.back());
			delegates.pop_back();
		}
	}

	const inline size_t subscriber_count() const { return delegates.size(); }

	
	inline void insert(const std::function<void(T...)> execute, size_t index) { if (execute) delegates.insert(delegates.begin() + index, execute); }

	Delegate(void) {};
	~Delegate(void) {};
};
