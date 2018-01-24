#pragma once
class Object;
class ComponentBase;

/// <summary>
/// Base Manager class for managers.
/// </summary>
class IManager {
protected:
	IManager() {} //IManager shouldn't exist on its own so make its ctor protected
public:
	virtual ~IManager() {}
};

/// <summary>
/// Interface for systems to access managers.
/// </summary>
/// <seealso cref="IManager" />
class IComponentManager: public IManager {
public:
	virtual ComponentBase* GetElement(const unsigned int _id) = 0;

	virtual ComponentBase* GetComponent(const char* filename) = 0;

	//virtual void CloneComponent(Object* source, Object* destination) = 0;
};
