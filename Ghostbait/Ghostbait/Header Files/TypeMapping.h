#pragma once
#include <unordered_map>
#include <atomic>

//TODO: FIX THIS LATER

class TypeMap {
	static std::atomic_uint lastTypeID; // the fact that this is static means it needs to be seperate.
	static std::atomic_uint lastObjectTypeID; // the fact that this is static means it needs to be seperate.
	static std::atomic_uint lastComponentTypeID; // the fact that this is static means it needs to be seperate.
protected:
	static std::unordered_map<std::string, unsigned> mapname;
	static std::unordered_map<std::string, unsigned> mapnameObject;
	static std::unordered_map<std::string, unsigned> mapnameComponent;
public:
	template <class Type>
	static unsigned GetTypeId() {
		static const unsigned id = lastTypeID++;
		return id;
	}
	template <class ObjectType>
	static unsigned GetObjectTypeID() {
		static const unsigned id = lastObjectTypeID++;
		return id;
	}
	template <class ComponentType>
	static unsigned GetComponentTypeID() {
		static const unsigned id = lastComponentTypeID++;
		return id;
	}

	static unsigned GetNameId(std::string &name) {
		return mapname[name];
	}

	static unsigned GetObjectNameID(std::string &_name) {
		return mapnameObject[_name];
	}

	static unsigned GetComponentNameID(std::string &_name) {
		return mapnameComponent[_name];
	}
	template <typename ComponentType>
	static void RegisterAlias(const std::string _name) {
		mapname[_name] = GetTypeId<ComponentType>();
	}

	template <typename ComponentType>
	static void RegisterObjectAlias(const std::string _name) {
		mapnameObject[_name] = GetObjectTypeID<ComponentType>();
	}

	template <typename ComponentType>
	static void RegisterComponentAlias(const std::string _name) {
		mapnameComponent[_name] = GetComponentTypeID<ComponentType>();
	}
};

