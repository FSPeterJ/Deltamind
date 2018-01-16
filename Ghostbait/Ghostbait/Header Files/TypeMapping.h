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
	template <class ComponentType>
	ComponentType* GetComponent()
	{
		return (ComponentType*)m_map[getTypeId<ComponentType>()];
	}

	template <class ComponentType>
	ComponentType* GetComponent() const { return m_map[getTypeId<ComponentType>()]; }

	template <class ComponentType>
	void AddComponent(ValueType* value) {
		m_map[getTypeId<ComponentType>()] = value;
	}
};

template <class ValueType>
std::atomic_int TypeMap<ValueType>::LastTypeId(0);