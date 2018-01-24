#pragma once
#include "TypeMapping.h"


template <class ValueType>
class TypeContainer : TypeMap{
	std::unordered_map<int, ValueType> typeMapID;
	using constIt = typename std::unordered_map<int, ValueType>::const_iterator;
	using baseIt = typename std::unordered_map<int, ValueType>::iterator;
public:
	//candidate for operator[]

	template <class ComponentType>
	ComponentType* GetComponent(const std::string &componentname)
	{
		baseIt loc = typeMapID.find(componentname);
		if(typeMapID.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent()
	{
		baseIt loc = typeMapID.find(getTypeId<ComponentType>());
		if(typeMapID.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent() const
	{
		constIt loc = typeMapID.find(getTypeId<ComponentType>());
		if(typeMapID.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent(const std::string &componentname) const
	{
		constIt loc = typeMapID.find(componentname);
		if(typeMapID.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	//operator +=
	template <class ComponentType>
	int AddComponent(ValueType value) {
		//m_mapname[std::string(typeid(ComponentType).name())] = value;
		typeMapID[getTypeId<ComponentType>()] = value;
		return typeMapID.find(getTypeId<ComponentType>())->first;
	}

	int AddComponent(ValueType value, const std::string &name) {
		typeMapID[mapname[name]] = value;
		return typeMapID.find(typeMapID[mapname[name]])->first;
	}

	int AddComponent(ValueType value, int id) {
		typeMapID[id] = value;
		return id;
	}


};

