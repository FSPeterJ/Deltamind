#pragma once
#include <unordered_map>
#include <atomic>


template <class ValueType>
class TypeMap {

	static std::atomic_int LastTypeId;

	static std::unordered_map<std::string, int> m_mapname;

	std::unordered_map<int, ValueType*> m_maptype;


	
	typedef typename std::unordered_map<int, ValueType*>::const_iterator constIt;
	typedef typename std::unordered_map<int, ValueType*>::iterator baseIt;
public:
	template <class ComponentType>
	static int getTypeId() {
		static const int id = LastTypeId++;
		return id;
	}

	template <typename ComponentType>
	static void RegisterComponent(std::string name) {

		m_mapname[std::string(name)] = getTypeId<ComponentType>();

	}

	template <class ComponentType>
	ComponentType* GetComponent(const std::string &componentname)
	{
		baseIt loc = m_mapname.find(componentname);
		if(m_mapname.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent()
	{
		baseIt loc = m_maptype.find(getTypeId<ComponentType>());
		if(m_maptype.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent() const
	{
		constIt loc = m_maptype.find(getTypeId<ComponentType>());
		if(m_maptype.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	ComponentType* GetComponent(const std::string &componentname) const
	{
		constIt loc = m_mapname.find(componentname);
		if(m_mapname.end() != loc) {
			return (ComponentType*)loc->second;
		}
		return nullptr;
	}

	template <class ComponentType>
	int AddComponent(ValueType* value) {
		//m_mapname[std::string(typeid(ComponentType).name())] = value;
		m_maptype[getTypeId<ComponentType>()] = value;
		return m_maptype.find(getTypeId<ComponentType>())->first;
	}

	//int AddComponent(ValueType* value, const std::string &name) {
	//	m_maptype[m_mapname[name]] = value;
	//	return m_maptype.find(m_maptype[m_mapname[name]])->first;
	//}


};

template <class ValueType>
std::atomic_int TypeMap<ValueType>::LastTypeId(0);

template <class ValueType>
std::unordered_map<std::string, int> TypeMap<ValueType>::m_mapname;

