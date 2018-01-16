#pragma once
#include <unordered_map>
#include <atomic>

template <class ValueType>
class TypeMap {

	static std::atomic_int LastTypeId;
	template <class ComponentType>
	static int getTypeId() {
		static const int id = LastTypeId++;
		return id;
	}

	std::unordered_map<int, ValueType*> m_map;
public:
	typedef typename std::unordered_map<int, ValueType*>::const_iterator constIt;
	typedef typename std::unordered_map<int, ValueType*>::iterator baseIt;
	template <class ComponentType>
	ComponentType* GetComponent()
	{
		baseIt loc = m_map.find(getTypeId<ComponentType>());
		if(m_map.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent() const
	{
		constIt loc = m_map.find(getTypeId<ComponentType>());
		if(m_map.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	void AddComponent(ValueType* value) {
		m_map[getTypeId<ComponentType>()] = value;
	}
};

template <class ValueType>
std::atomic_int TypeMap<ValueType>::LastTypeId(0);