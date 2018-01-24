#pragma once
#include "Object.h"
#include "Delegate.h"
#include "EngineStructure.h"
#include "Controlable.h"
#include "PhysicsManager.h"
#include "GameObject.h"

GameObject::GameObject() {
	
}

void GameObject::Activate() {
	EngineStructure::Awake += [=]() {this->Awake(); };
}

static int i = 0;
void LeftControllerObject::Update() {
	Object* something = this;
	i++;
	if(KeyIsDown(leftAttack)) {
		++i;
		if(i >= 60) {
			WriteLine("Got left attack as left controller");
			//ResetKey(leftAttack);
			Object* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, {0, 0, 0, 0}, &obj));
			obj->position = position;
			//obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 3.0f, position._32 * 3.0f, position._33 * 3.0f);
			i = 0;
		}
	}
}
void RightControllerObject::Update() {
	if(KeyIsDown(rightAttack)) {
		//WriteLine("Got right attack as right controller");
		//ResetKey(rightAttack);
		//DirectX::XMFLOAT4 temp(VRManager::leftController.pose._41, VRManager::leftController.pose._42, VRManager::leftController.pose._43, VRManager::leftController.pose._44);
		//Object* obj;
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, temp, &obj));
		//obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 3.0f, position._32 * 3.0f, position._33 * 3.0f);
	}
}
