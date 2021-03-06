#pragma once
#include <DirectXMath.h> //todo get rid of
#include "TypeContainer.h"
#include "StdHeader.h" //for PrefabId
#include "Delegate.h"
#include "Transform.h"
#include "ComponentBase.h"

//
//namespace DirectX {
//
//	struct XMFLOAT4X4;
//}

class Object {
protected:

private:

public:
	Transform transform;

	Delegate<> DestroyComponents;
	ComponentTypeContainer<ComponentBase*> Components;
	//Endnote
	ComponentBase* defaultMat = nullptr;
	std::unordered_map<std::string, ComponentBase*> componentVariants;
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

	template<typename ComponentType>
	bool SwapComponentVariant(const char* variantName) {
		if (componentVariants.find(variantName) != componentVariants.end()) {
			int id = TypeMap::GetComponentTypeID<ComponentType>();
			SetComponent(componentVariants[variantName], id);
			return true;
		}
		return false;
	}

	virtual void GiveComponent(ComponentBase * _component, const char* tag);

	virtual void GivePID(unsigned pid, const char* tag);
	//virtual void SmokeTest();
#ifdef _DEBUG
	virtual void SmokeTest();
#endif
};
