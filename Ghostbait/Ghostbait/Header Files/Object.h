#pragma once
#include <directxmath.h>
#include "MessageEvents.h"
#include "ComponentBase.h"
#include "TypeContainer.h"

class Object {
protected:

private:

public:
	//TODO: convert to XMFLAOT4x4
	DirectX::XMFLOAT4X4 position;
	TypeContainer<ComponentBase*> Components;
	//Endnote

	Object() {
		DirectX::XMStoreFloat4x4(&position, DirectX::XMMatrixIdentity());
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
