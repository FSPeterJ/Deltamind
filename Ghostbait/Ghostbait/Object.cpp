#include "Object.h"

std::unordered_map<std::string, Object*> Object::protoTable;

Pool Object::objectPool = Pool(2,1);



Object* Object::AddPrototype(std::string type, Object* obj) {
//	Debug("Adding prototype for " << type.c_str() << " : " << obj);

	protoTable[type] = obj;

	//Debug("I just made a prototype : " << obj);

	return obj;
}

std::string Object::GetObjectTypeName(Object* o) {
	std::stringstream ss(typeid(*o).name());
	std::string res;
	ss >> res; ss >> res;
	return res;
}
