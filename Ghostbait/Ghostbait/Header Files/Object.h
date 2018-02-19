#pragma once
#include <DirectXMath.h> //todo get rid of
#include "TypeContainer.h"
#include "StdHeader.h" //for PrefabId
#include "Delegate.h"

class ComponentBase;
//
//namespace DirectX {
//
//	struct XMFLOAT4X4;
//}

class Object {
protected:

private:

public:

	Delegate<> DestroyComponents;
	DirectX::XMFLOAT4X4 position; //if we make this a pointer we don't have to include DXMath into every single object
	ComponentTypeContainer<ComponentBase*> Components;
	//Endnote

	Object();

	virtual ~Object();

	template<typename T>
	static T* Create(DirectX::XMFLOAT4 position, const PrefabId id) {
		//T* newObject = nullptr;
		//how to get id from type??
		//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage(id, position, &newObject));
		//return newObject;
	}

	template<typename ComponentType>
	ComponentType* GetComponent() {
		return Components.GetComponent<ComponentType>();
	}

	template<typename ComponentType>
	ComponentType* GetComponent() const {
		return Components.GetComponent<ComponentType>();
	}

	template<typename ComponentType>
	int SetComponent(ComponentType * _component) {
		return Components.AddComponent<ComponentType>(_component);
	}

	int SetComponent(ComponentBase* _component, const int _id);

	//ComponentBase* GetComponent(const std::string &componentname)
	//{
	//	return Components.GetComponent<>(componentname);
	//};


	virtual void GivePID(unsigned pid, char* tag);
	virtual void CloneData(Object* obj);

};
