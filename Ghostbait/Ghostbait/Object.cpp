#include "Object.h"

std::unordered_map<std::string, Object*> Object::protoTable;

Pool Object::objectPool = Pool(100,100);



Object* Object::AddPrototype(std::string type, Object* obj) {
	Console::WriteLine("Adding prototype for " + type);
	protoTable[type] = obj;
	return obj;
}

std::string Object::GetObjectTypeName(Object* o) {
	std::stringstream ss(typeid(*o).name());
	std::string res;
	ss >> res; ss >> res;
	return res;
}
