#pragma once
#include <DirectXMath.h>
#include "TypeContainer.h"
#include "MessageEvents.h"

class ComponentBase;

class GhostMatrix : public DirectX::XMFLOAT4X4 {
	DirectX::XMFLOAT4X4 previous = DirectX::XMFLOAT4X4(0, 0, 0, 0, 
														0, 0, 0, 0, 
														0, 0, 0, 0, 
														0, 0, 0, 0);
public:
	inline const DirectX::XMFLOAT4X4 Previous() const { return previous; };
	bool operator!=(const DirectX::XMFLOAT4X4& other) {
		if (this->_11 != other._11) return true;
		if (this->_12 != other._12) return true;
		if (this->_13 != other._13) return true;
		if (this->_14 != other._14) return true;
		if (this->_21 != other._21) return true;
		if (this->_22 != other._22) return true;
		if (this->_23 != other._23) return true;
		if (this->_24 != other._24) return true;
		if (this->_31 != other._31) return true;
		if (this->_32 != other._32) return true;
		if (this->_33 != other._33) return true;
		if (this->_34 != other._34) return true;
		if (this->_41 != other._41) return true;
		if (this->_42 != other._42) return true;
		if (this->_43 != other._43) return true;
		if (this->_44 != other._44) return true;
		return false;
	}
	void operator=(const DirectX::XMFLOAT4X4& other) {
		previous = *this;
		*this = other;
	}
};

class Object {
protected:

private:

public:
	GhostMatrix position;
	Delegate<> DestroyComponents;
	ComponentTypeContainer<ComponentBase*> Components;
	//Endnote


	Object() {
		XMStoreFloat4x4(&position, DirectX::XMMatrixIdentity());
		//Example of string registration
		//Use this to associate strings to same typeid
		//This MUST be called SOMEWHERE in order for Factory to know what object to grab
		//ObjectTypeMap::RegisterObject<Object>("Object");
		//ObjectTypeMap::RegisterObject<Object>("object");
		//ObjectTypeMap::RegisterObject<Object>("obj");

	};
	virtual ~Object() {
		//int x = 111;
	};

	template<typename T>
	static T* Create(DirectX::XMFLOAT4 position, const PrefabId id) {

		
		//T* newObject = nullptr;
		//how to get id from type??
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(id, position, &newObject));
		//return newObject;
	}

	template<typename ComponentType>
	ComponentType* GetComponent() {
		
		
		return Components.GetComponent<ComponentType>();
		
	};

	template<typename ComponentType>
	ComponentType* GetComponent() const {
		return Components.GetComponent<ComponentType>();
	};

	//ComponentBase* GetComponent(const std::string &componentname)
	//{
	//	return Components.GetComponent<>(componentname);
	//};

	template<typename ComponentType>
	int SetComponent(ComponentType * _component) {
		return Components.AddComponent<ComponentType>(_component);
	};

	int SetComponent(ComponentBase* _component, const int _id) {
		return Components.AddComponent(_component, _id);
	};

	//always return 0?
	static int GetTypeId() { return 0; };
};
