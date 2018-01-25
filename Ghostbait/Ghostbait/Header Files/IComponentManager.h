#pragma once

class Object;
class ComponentBase;

//IManager shouldn't exist on its own so make its ctor protected
class IManager {
protected:
	IManager() {}
public:
	virtual ~IManager() {}
};

//Interface for systems to access managers
class IComponentManager: public IManager {
public:
	virtual ComponentBase* GetReferenceComponent(const char * _FilePath, const char * _data) = 0;
	virtual ComponentBase* CloneComponent(ComponentBase* reference) = 0;
};
