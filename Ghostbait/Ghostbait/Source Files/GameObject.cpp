#pragma once
#include "GameObject.h"
#include "GhostTime.h"
#include "EngineStructure.h"
#include "MessageEvents.h"

GameObject::GameObject() {
	//updateID = EngineStructure::Update.Add([=]() {this->Update(); });
}

void GameObject::OnCollision(GameObject* obj) {
}

//This is potentially dangerous if used incorrectly.  
//Double Enable emplaces an update delegate that can never be removed.  
void GameObject::Enable() {
	//If check was added to prevent user error, but may be unecessary
	if(!updateID) {
		//Profile for if adding a delegate has any performance impact +/-
		// Iterating & checking states of hundreds of pool active items per frame vs adding / removing one or two delegates every handful of frames
		updateID = EngineStructure::Update.Add([=]() {this->Update(); });
	}
}

void GameObject::Disable() {
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {EngineStructure::Update.Remove(updateID); updateID = 0; });
}

void GameObject::DisableLate() {
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

void MenuCube::Update() {
	position.m[3][1] += 0.4f * (float)GhostTime::DeltaTime();
	if(position.m[3][1] > 1.5f) {
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
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(8/*Core*/, { 0, 1.5f, 0 }, &obj));
		DirectX::XMStoreFloat4x4(&obj->position,
			DirectX::XMLoadFloat4x4(&obj->position) * DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f));
	}
}



void CoreCube::OnCollision(GameObject* other) {
	if(other->GetTag() == "enemy") {
		Console::WriteLine << "YOU LOSE!";
		Console::OutLine << "YOU LOSE!";
		MessageEvents::SendQueueMessage(EVENT_Late, [=] {this->Destroy(); });
		GameObject* temper;
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(10/*LoseCube*/, { 0, 0.75f, 0 }, &temper));
		DirectX::XMStoreFloat4x4(&temper->position, DirectX::XMLoadFloat4x4(&temper->position) * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));
		DirectX::XMStoreFloat4x4(&temper->position,	DirectX::XMLoadFloat4x4(&temper->position) * DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));
	}
}
