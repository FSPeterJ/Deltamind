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

void LeftControllerObject::Update() {
	if (KeyIsDown(leftItem1)) {
		state = GUN;
		ResetKey(leftItem1);
	}
	else if (KeyIsDown(leftItem2)) {
		state = CONTROLLER;
		ResetKey(leftItem2);
	}
	else if (KeyIsDown(leftItem3)) {
		state = HAND;
		ResetKey(leftItem3);
	}
	else {

	}

	switch (state) {
	case GUN:
		if (KeyIsDown(leftAttack)) {
			WriteLine("Left gun shoot");
			ResetKey(leftAttack);
			Object* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, {0, 0, 0, 0}, &obj));
			obj->position = position;
			obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
			obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 0.1f, position._32 * 0.1f, position._33 * 0.1f);
		}
		break;
	case CONTROLLER:
		if (KeyIsDown(leftAttack)) {
			WriteLine("Left controller select");
			ResetKey(leftAttack);
		}
		break;
	case HAND:
		if (KeyIsDown(leftAttack)) {
			WriteLine("Left hand pickup");
			ResetKey(leftAttack);
		}
		break;
	}
}
void RightControllerObject::Update() {

	if (KeyIsDown(rightItem1)) {
		state = GUN;
		ResetKey(rightItem1);
	}
	else if (KeyIsDown(rightItem2)) {
		state = CONTROLLER;
		ResetKey(rightItem2);
	}
	else if (KeyIsDown(rightItem3)) {
		state = HAND;
		ResetKey(rightItem3);
	}
	else {

	}

	switch (state) {
	case GUN:
		if (KeyIsDown(rightAttack)) {
			WriteLine("Right gun shoot");
			ResetKey(rightAttack);
			Object* obj;
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(1, { 0, 0, 0, 0 }, &obj));
			obj->position = position;
			obj->GetComponent<PhysicsComponent>()->rigidBody.AdjustGravityMagnitude(0);
			obj->GetComponent<PhysicsComponent>()->rigidBody.SetVelocity(position._31 * 0.1f, position._32 * 0.1f, position._33 * 0.1f);
		}
		break;
	case CONTROLLER:
		if (KeyIsDown(rightAttack)) {
			WriteLine("Right controller select");
			ResetKey(rightAttack);
		}
		break;
	case HAND:
		if (KeyIsDown(rightAttack)) {
			WriteLine("Right hand pickup");
			ResetKey(rightAttack);
		}
		break;
	}
}
