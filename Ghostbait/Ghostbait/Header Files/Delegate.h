#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <cassert>


template <typename ...T>
class Delegate: std::function<void(T...)> {

	struct Delegate_Entry {
		unsigned id;
		bool active = true;
		std::function<void(T...)> function;
		bool operator ==(const Delegate_Entry &B) const {
			return id == B.id;
		}

		bool operator () (const Delegate_Entry& B) const {
			return id == B.id;
		}

	};

	std::vector<Delegate_Entry> delegate_list;
	unsigned lastID = 0;
public:
	void operator()(const T&... e) const {
		
		for(const Delegate_Entry &element : delegate_list) {
			//It better to just iterate a pointer list than check 1,500 times.  Fix on next pass
			if(element.active) {
				//If you crashed here with some nonsensical memory access error you probably removed something from the delegates while iterating them at the same time
				// IE: In an object's updated method you tell the engine to Remove(myupdateID)
				// If this is a serious problem, ask to transform this into an iterator loop with special function calls for mid-access deletions
				// or use late update for the removal
				element.function(e...);
			}
		}
	}
	//Possibly want to phase this out.  Unsure what problems we will run into with 0 being an ID for permament registration
	void operator+=(const std::function<void(T...)> execute) {
		Delegate_Entry data = { 0, true, execute };
		delegate_list.push_back(data);
	}

	unsigned Add(const std::function<void(T...)> execute) {
		Delegate_Entry data = { ++lastID, true, execute };
		Delegate_Entry datas = { ++lastID, true, execute };
		if(data == datas) {

		}
		delegate_list.push_back(data);
		return data.id;
	}

	//Will be added in sleep mode on, must be activated
	unsigned AddAsleep(const std::function<void(T...)> execute) {
		Delegate_Entry data = { ++lastID, false, execute };
		delegate_list.push_back(data);
		return data.id;
	}
	// False data can probably be replaced with raw unsigned if the operators are made correctly for the struct package
	void Sleep(unsigned id) {
		Delegate_Entry data = { id, true, nullptr };
		find(delegate_list.begin(), delegate_list.end(), data)->active = false;
	}

	void Awake(unsigned id) {
		Delegate_Entry data = { id, true, nullptr };
		find(delegate_list.begin(), delegate_list.end(), data)->active = true;
	}


	void Remove(unsigned id) {
		delegate_list.begin();
		Delegate_Entry data = { id, true, nullptr };
		std::vector<Delegate_Entry>::iterator it = find(delegate_list.begin(), delegate_list.end(), data);

		if(it != delegate_list.end() - 1) {
			std::swap(*it, delegate_list.back());
		}
		delegate_list.pop_back();
		delegate_list.begin();
	}

	const inline size_t subscriber_count() const { return delegate_list.size(); }

	inline void insert(const std::function<void(T...)> execute, size_t index) { delegate_list.insert(delegate_list.begin() + index, execute); }

	Delegate(void) {};
	~Delegate(void) {};
};
