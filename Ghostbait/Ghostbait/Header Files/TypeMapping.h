#pragma once
#include <unordered_map>
#include <atomic>

//TODO: FIX THIS LATER

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

	static unsigned GetNameId(std::string &name) {
		return mapname[name]; //Will return 0 if name is not registered
	}

	static unsigned GetObjectNameID(std::string &_name) {
		return mapnameObject[_name];//Will return 0 if name is not registered
	}

	static unsigned GetComponentNameID(std::string &_name) {
		return mapnameComponent[_name];//Will return 0 if name is not registered
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
