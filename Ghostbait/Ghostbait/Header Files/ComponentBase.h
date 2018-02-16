#pragma once

class Object;

/// <summary>
/// Allows for covariant return types in Manager interface
/// </summary>
class ComponentBase {
public:
	bool singleInstance = true;
	//If you are creating this base, you are doing something wrong.
	//to prevent creation of this class, mark its ctor protected. now it can't exist on its own
protected:
	ComponentBase() {}
};

class InstantiatedCompBase: public ComponentBase {
public:
	InstantiatedCompBase() {
		singleInstance = false;
	}
	 Object* parentObject = nullptr;
};
