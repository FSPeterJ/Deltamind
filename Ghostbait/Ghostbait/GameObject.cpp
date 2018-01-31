#pragma once
#include "Object.h"
#include "Delegate.h"
#include "EngineStructure.h"
#include "GameObject.h"
#include "MessageEvents.h"
#include "GhostTime.h"
#include "Console.h"
#include "functional"


GameObject::GameObject()
{
	updateID = EngineStructure::Update.AddAsleep([=]() {this->Update(); });
}

void GameObject::OnCollision(GameObject* obj) {
}

void GameObject::Enable() {
	EngineStructure::Update.Awake(updateID);
}

void GameObject::Disable() {
	//MessageEvents::SendQueueMessage(EVENT_Late, [=]() {this->Update(); });
	//MessageEvents::SendQueueMessage(EVENT_Late,  [updateID]() { EngineStructure::Update.Remove(updateID); });
	//MessageEvents::SendQueueMessage<unsigned>(EVENT_Late, std::function( [updateID]() { EngineStructure::Update.Remove(updateID); }));
	/*MessageEvents::SendQueueMessage(EVENT_Late, std::function( [updateID]() { EngineStructure::Update.Remove(updateID); }));
	MessageEvents::SendQueueMessage(EVENT_Late, std::function( [=]() { EngineStructure::Update.Remove(updateID); }));
	MessageEvents::SendQueueMessage(EVENT_Late, [updateID]() { EngineStructure::Update.Remove(updateID); });
	MessageEvents::SendQueueMessage(EVENT_Late, [=]() { EngineStructure::Update.Remove(updateID); });*/
	EngineStructure::Update.Sleep(updateID);

	
}

void GameObject::Destroy() {
	//recycle memory, pool::deactivatememory
	MessageEvents::SendMessage(EVENT_Destroy, DestroyMessage(this));
	
	DestroyComponents();
	Disable();
	Components.Clear();
}

void MenuCube::Update()
{
	position.m[3][1] += 0.2f * (float)GhostTime::DeltaTime();
	if(position.m[3][1] > 2.5f)
	{
		Disable();
	}
}

void MenuCube::OnCollision(GameObject* other) {
	if(other->GetTag() == "Bullet") {
		Destroy();
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 10, 0, 0 }));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { -10, 0, 0 }));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 0, 0, 10 }));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(5/*Spawner*/, { 0, 0, -10 }));
		GameObject* obj;
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(8/*Core*/, { 0, 1.5f, 0 }, (Object**)&obj));
		DirectX::XMStoreFloat4x4(&obj->position,
			DirectX::XMLoadFloat4x4(&obj->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
	}
}
