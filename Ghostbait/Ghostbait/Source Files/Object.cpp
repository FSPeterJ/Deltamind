#include "Object.h"

Object::Object() {
	XMStoreFloat4x4(&position, DirectX::XMMatrixIdentity());
	//Example of string registration
	//Use this to associate strings to same typeid
	//This MUST be called SOMEWHERE in order for Factory to know what object to grab
	//ObjectTypeMap::RegisterObject<Object>("Object");
	//ObjectTypeMap::RegisterObject<Object>("object");
	//ObjectTypeMap::RegisterObject<Object>("obj");

}

Object::~Object() {
	//int x = 111;
}

int Object::SetComponent(ComponentBase* _component, const int _id) {
	return Components.AddComponent(_component, _id);
}


void Object::GiveComponent(ComponentBase* _component, const char* tag) {
	componentVarients[tag] = _component;
}

void Object::GivePID(unsigned pid, const char* tag)
{
}

void Object::CloneData(Object* obj)
{

}

void Object::SmokeTest() {
}

