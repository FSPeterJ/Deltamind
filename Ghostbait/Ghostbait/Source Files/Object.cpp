#include "Object.h"

Object::Object() {
	DirectX::XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, DirectX::XMMatrixIdentity());
	transform.SetMatrix(temp);
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
	componentVariants[tag] = _component;
}
void Object::GivePID(unsigned pid, const char* tag)
{
}



#ifdef _DEBUG
void Object::SmokeTest() {
}
#endif
/*
void Object::SetPosition(DirectX::XMFLOAT4X4 newPos) {
	position = newPos;
}
const DirectX::XMFLOAT4X4& Object::GetPosition() const {
	return position;
}
*/