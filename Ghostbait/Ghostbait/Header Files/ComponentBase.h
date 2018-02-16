#pragma once

class Object;

/// <summary>
/// Allows for covariant return types in Manager interface
/// </summary>
class ComponentBase {
public:
	bool singleInstance = true;
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
