#pragma once
#include "GameObject.h"
#include "GhostTime.h"
#include "EngineStructure.h"
#include "MessageEvents.h"

GameObject::GameObject() {
	//updateID = EngineStructure::Update.Add([=]() {this->Update(); });
}

void GameObject::OnCollision(GameObject* obj) {}


void GameObject::Enable() {
	Awake();
	//This is potentially dangerous if used incorrectly.
	//Double Enable emplaces a second update delegate that can never be removed.
	MessageEvents::Subscribe(EVENT_RestartGame, [=](EventMessageBase* e) {this->RestartGame(); });
	//If check was added to prevent user error, but may be unecessary
	if(!updateID) {
		//Profile for if adding a delegate has any performance impact +/-
		// Iterating & checking states of hundreds of pool active items per frame vs adding / removing one or two delegates every handful of frames
		updateID = EngineStructure::Update.Add([=]() { Update(); });
	}
}

// Will disable the object after Update main loop is complete
void GameObject::Disable() {
	// "Bad ID given.  You cannot remove permament delegates (ID of 0)";
	//assert(updateID != 0);

	MessageEvents::SendQueueMessage(EVENT_Late, [=] {
		if(updateID != 0) {
			// This is because many people in the same update loop can tell you "hey, disable yourself"
			// but only the first execution will disable the correct one. I do not have a quick solution to this problem
			// Possibly confirming with a local bool or bitset for state information would be better than constructing a redundant lambda, but then this may be confusing and hard to debug later
			// (Object tests as active half way through the update loop vs Object consistently tests as active the whole update loop and then is disabled with LateUpdate)
			EngineStructure::Update.Remove(updateID);
			updateID = 0;
		}
	});
}

void GameObject::DisableNow() {
	EngineStructure::Update.Remove(updateID);
	updateID = 0;
}

void GameObject::Destroy() {
	//recycle memory, pool::deactivatememory
	MessageEvents::SendMessage(EVENT_Destroy, DestroyMessage(this));
	DestroyComponents();
	Components.Clear();
	Disable();
}


void GameObject::PassObject(GameObject* obj, char* tag)
{
}

void MenuCube::Update() {
	position.m[3][1] += 0.4f * (float) GhostTime::DeltaTime();
	if(position.m[3][1] > 1.5f) {
		Disable();
	}
}

void MenuCube::OnCollision(GameObject* other) {
	if(other->GetTag() == "Bullet") {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
		MessageEvents::SendMessage(EVENT_StartWave, EventMessageBase());
		//GameObject* obj;
		//MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage(8/*Core*/, {0, 1.5f, 0}, &obj));
		//DirectX::XMStoreFloat4x4(&obj->position,
		//	DirectX::XMLoadFloat4x4(&obj->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
	}
}

void CoreCube::OnCollision(GameObject* other) {
	if(other->GetTag() == "enemy") {
		Console::WriteLine << "YOU LOSE!";
		Console::OutLine << "YOU LOSE!";
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
		GameObject* temper;
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(10, {0, 0.75f, 0}, &temper));// stop using magic number prefab ID
		DirectX::XMStoreFloat4x4(&temper->position, DirectX::XMLoadFloat4x4(&temper->position) * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));
	}
}
