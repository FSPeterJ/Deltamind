#pragma once
#include "TypeMapping.h"

template <class ValueType>
class TypeContainer: TypeMap {
	std::unordered_map<unsigned, ValueType> typeMapID;
	using constIt = typename std::unordered_map<unsigned, ValueType>::const_iterator;
	using baseIt = typename std::unordered_map<unsigned, ValueType>::iterator;
public:
	//candidate for operator[]

	template <class ComponentType>
	ComponentType* GetComponent(const std::string &componentname) {
		baseIt loc = typeMapID.find(componentname);
		if(typeMapID.end() != loc) {
			return (ComponentType*) loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent() {
		baseIt loc = typeMapID.find(GetTypeId<ComponentType>());
		if(typeMapID.end() != loc) {
			return (ComponentType*) loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent() const {
		constIt loc = typeMapID.find(GetTypeId<ComponentType>());
		if(typeMapID.end() != loc) {
			return (ComponentType*) loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent(const std::string &componentname) const {
		constIt loc = typeMapID.find(componentname);
		if(typeMapID.end() != loc) {
			return (ComponentType*) loc->second;
		}
		return nullptr;
	}

	//operator +=
	template <class ComponentType>
	unsigned AddComponent(ValueType value) {
		//m_mapname[std::string(typeid(ComponentType).name())] = value;
		typeMapID[GetTypeId<ComponentType>()] = value;
		return typeMapID.find(GetTypeId<ComponentType>())->first;
	}

	unsigned AddComponent(ValueType value, const std::string &name) {
		typeMapID[mapname[name]] = value;
		return typeMapID.find(typeMapID[mapname[name]])->first;
	}

	unsigned AddComponent(ValueType value, unsigned id) {
		typeMapID[id] = value;
		return id;
	}
};

template <class ValueType>
class ComponentTypeContainer: TypeMap {
	std::unordered_map<unsigned, ValueType> typeMapID;
	using constIt = typename std::unordered_map<unsigned, ValueType>::const_iterator;
	using baseIt = typename std::unordered_map<unsigned, ValueType>::iterator;
public:
	//candidate for operator[]

	template <class ComponentType>
	ComponentType* GetComponent(const std::string &componentname) {
		baseIt loc = typeMapID.find(componentname);
		if(typeMapID.end() != loc) {
			return (ComponentType*) loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent() {
		baseIt loc = typeMapID.find(GetComponentTypeID<ComponentType>());
		if(typeMapID.end() != loc) {
			return (ComponentType*) loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent() const {
		constIt loc = typeMapID.find(GetComponentTypeID<ComponentType>());
		if(typeMapID.end() != loc) {
			return (ComponentType*) loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent(const std::string &componentname) const {
		constIt loc = typeMapID.find(componentname);
		if(typeMapID.end() != loc) {
			return (ComponentType*) loc->second;
		}
		return nullptr;
	}

	void Clear() {
		typeMapID.clear();
	}

	//operator +=
	template <class ComponentType>
	unsigned AddComponent(ValueType value) {
		//m_mapname[std::string(typeid(ComponentType).name())] = value;
		typeMapID[GetTypeId<ComponentType>()] = value;
		return typeMapID.find(GetComponentTypeID<ComponentType>())->first;
	}

	unsigned AddComponent(ValueType value, const std::string &name) {
		typeMapID[mapname[name]] = value;
		return typeMapID.find(typeMapID[mapname[name]])->first;
	}

	unsigned AddComponent(ValueType value, unsigned id) {
		typeMapID[id] = value;
		return id;
	}
};
