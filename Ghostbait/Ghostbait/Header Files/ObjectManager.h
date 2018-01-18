#pragma once
#include "ManagerInterface.h"
#include <vector>
#include "Object.h"
#include <unordered_map>
#include "MessageEvents.h"
#include <array>


class ObjectManager: public ManagerInterface {
private:
	
	/*
	class Pool {
	private:
		std::vector<unsigned> activeList;
		std::vector<unsigned> inactiveList; //Linked list with head/tail ptrs
		//std::vector<Object> objects;


		void* objects;
		int pool_count;
		int max_size;

		template<typename T>
		void RemoveObjectFromActive(const T* o) {
			//auto it = std::find(activeList.begin(), activeList.end(), o);

			//if(it != activeList.end()) {
				//std::swap(*it, activeList.back());
			//	activeList.pop_back();
			//}
		}
		template<typename T>
		T* Add(const T* o) {
		//	objects[pool_count] = *o;
			//pool_count++;
			//activeList.push_back(&objects[pool_count]);
			return 0;// &objects[pool_count];
		}
	public:
		//template<typename T>
		//Pool() {
		//	objects = new T[10];
		//}

		Pool() {
			
		}
		/// <summary>
		/// Activates the specified object.
		/// </summary>
		/// <param name="o">The object to activate.</param>
		/// <returns>A pointer to the activated or added Object.</returns>
		template<typename T>
		unsigned Activate(const T* o) {
			if(inactiveList.size()) {
			//	activeList.push_back(inactiveList[0]);
				//inactiveList.erase(inactiveList.begin());
				return 0;// activeList.back();
			}
			else {
				return 0;// Add(o);
			}
		}

		/// <summary>
		/// Deactivates the specified object.
		/// </summary>
		/// <param name="o">The object to deactivate.</param>
		template<typename T>
		void Deactivate(T* o) {
			//RemoveObjectFromActive(o);
//			inactiveList.push_back(o);
		}
	};

	static std::vector<Pool> objectPool;
	static std::unordered_map<const Object*, Pool*> poolScope;
	*/
	static void Instantiate(EventMessageBase* e);
	static void Destroy(EventMessageBase* e);

	ObjectManager();
public:
	~ObjectManager();
	static void Initialize();
	static void CreatePool(int _size, Object* poolType) {}
};


