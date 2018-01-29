#pragma once
#include "Object.h"
#include "Delegate.h"
#include "EngineStructure.h"
#include "GameObject.h"
#include "MessageEvents.h"

GameObject::GameObject() {}
void GameObject::OnCollision(GameObject* obj) {

}
void GameObject::Enable() {
	EngineStructure::Awake += [=]() {this->Awake(); };
	isAwake = true;
}
void GameObject::Disable() {
	isAwake = false;
	//unsub from delegates
}
void GameObject::Destroy() {
	//recycle memory, pool::deactivatememory
	MessageEvents::SendMessage(EVENT_Destroy, DestroyMessage(this));

	DestroyComponents();
	Disable();
	Components.Clear();
}
