#pragma once
#include <unordered_map>
#include <atomic>

//TODO: FIX THIS LATER

class TypeMap {
	static std::atomic_int lastTypeID; // the fact that this is static means it needs to be seperate.
	static std::atomic_int lastObjectTypeID; // the fact that this is static means it needs to be seperate.
	static std::atomic_int lastComponentTypeID; // the fact that this is static means it needs to be seperate.
protected:
	static std::unordered_map<std::string, int> mapname;
	static std::unordered_map<std::string, int> mapnameObject;
	static std::unordered_map<std::string, int> mapnameComponent;
public:
	template <class Type>
	static int GetTypeId() {
		static const int id = lastTypeID++;
		return id;
	}
	template <class ObjectType>
	static int GetObjectTypeID() {
		static const int id = lastObjectTypeID++;
		return id;
	}
	template <class ComponentType>
	static int GetComponentTypeID() {
		static const int id = lastComponentTypeID++;
		return id;
	}

	static int GetNameId(std::string &name) {
		return mapname[name];
	}

	static int GetObjectNameID(std::string &_name) {
		return mapnameObject[_name];
	}

	static int GetComponentNameID(std::string &_name) {
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

