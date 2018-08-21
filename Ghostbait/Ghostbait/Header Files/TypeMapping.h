#pragma once
#include <unordered_map>
#include <atomic>

//This was an odd test of custom sorta-RTTI with template metaprogramming (Most of it is done at compile time).  
//The main goal was to have a system for indexing into manageable sized arrays by type while also being able to access them through multiple different strings if necessary.
//Honestly it was done more for fun but it works alright.

//Note: you should not use any string comparisons durring the game loop runtime.  Only during setup and initialization, store a local index number if needed.

class TypeMap {
	static std::atomic_uint lastTypeID;
	static std::atomic_uint lastObjectTypeID;
	static std::atomic_uint lastComponentTypeID;
protected:
	static std::unordered_map<std::string, unsigned> mapname;
	static std::unordered_map<std::string, unsigned> mapnameObject;
	static std::unordered_map<unsigned, std::string> mapnameObjectReverse;
	static std::unordered_map<std::string, unsigned> mapnameComponent;
public:
	template <class Type>
	constexpr static unsigned GetTypeId() {
		static const unsigned id = lastTypeID++;
		return id;
	}
	template <class ObjectType>
	constexpr static unsigned GetObjectTypeID() {
		static const unsigned id = lastObjectTypeID++;
		return id;
	}
	template <class ComponentType>
	constexpr static unsigned GetComponentTypeID() {
		static const unsigned id = lastComponentTypeID++;
		return id;
	}
	//Will return 0 if name is not registered
	static unsigned GetNameId(std::string &name) {
		return mapname[name]; 
	}
	//Will return 0 if name is not registered
	static unsigned GetObjectNameID(std::string &_name) {
		return mapnameObject[_name];
	}
	//Will return 0 if name is not registered
	static unsigned GetComponentNameID(std::string &_name) {
		return mapnameComponent[_name];
	}
	template <typename ComponentType>
	static void RegisterAlias(const std::string _name) {
		mapname[_name] = GetTypeId<ComponentType>();

	}

	static const char* GetObjectNameFromID(unsigned id) {
		return mapnameObjectReverse[id].c_str();
	}

	template <typename ComponentType>
	static void RegisterObjectAlias(const std::string _name) {
		mapnameObject[_name] = GetObjectTypeID<ComponentType>();
		mapnameObjectReverse[GetObjectTypeID<ComponentType>()] = _name;
	}

	template <typename ComponentType>
	static void RegisterComponentAlias(const std::string _name) {
		mapnameComponent[_name] = GetComponentTypeID<ComponentType>();
	}
};
