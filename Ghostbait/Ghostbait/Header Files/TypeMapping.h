#pragma once
#include <unordered_map>
#include <atomic>


class TypeMap {
	static std::atomic_int lastTypeID;
protected:
	static std::unordered_map<std::string, int> mapname;
public:
	template <class ComponentType>
	static int getTypeId() 
	{
		static const int id = lastTypeID++;
		return id;
	}

	static int getNameId(std::string &name)
	{
		return mapname[name];
	}
	template <typename ComponentType>
	static void RegisterComponent(const std::string name) 
	{

		mapname[name] = getTypeId<ComponentType>();
	}
};



