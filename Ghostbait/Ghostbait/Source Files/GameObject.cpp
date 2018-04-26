#pragma once
#include "GameObject.h"
#include "GhostTime.h"
#include "EngineStructure.h"
#include "MessageEvents.h"

GameObject::GameObject() {
	//updateID = EngineStructure::Update.Add([=]() {this->Update(); });
}

void GameObject::PersistentSubscribe() {
	if(!eventDeleteAllGameObjects) {
		eventDeleteAllGameObjects = MessageEvents::Subscribe(EVENT_DeleteAllGameObjects, [=](EventMessageBase* e) {
			MessageEvents::SendQueueMessage(EVENT_Late, [=] {
				this->Destroy();
			});
		});
	}
}

void GameObject::PersistentUnSubscribe() {
	if(eventDeleteAllGameObjects) {
		MessageEvents::UnSubscribe(EVENT_DeleteAllGameObjects, eventDeleteAllGameObjects);
		eventDeleteAllGameObjects = 0;
	}
}

void GameObject::PersistOnReset() {
	flags |= PERSIST_ON_RESET;
	if(eventDeleteAllGameObjects) {
		MessageEvents::UnSubscribe(EVENT_DeleteAllGameObjects, eventDeleteAllGameObjects);
		eventDeleteAllGameObjects = 0;
	}
}

void GameObject::DestroyOnReset() {
	flags &= ~PERSIST_ON_RESET;
	if(!eventDeleteAllGameObjects) {
		eventDeleteAllGameObjects = MessageEvents::Subscribe(EVENT_DeleteAllGameObjects, [=](EventMessageBase* e) {
			MessageEvents::SendQueueMessage(EVENT_Late, [=] {
				this->Destroy();
			});
		});
	}
}

void GameObject::Subscribe() {

}
void GameObject::UnSubscribe() {

}
void GameObject::Enable() {
	GameObject::Subscribe();
	//This is potentially dangerous if used incorrectly.
		//Double Enable emplaces a second update delegate that can never be removed.
		//If check was added to prevent user error, but may be unecessary
	if(!updateID) {
		//Profile for if adding a delegate has any performance impact +/-
			// Iterating & checking states of hundreds of pool active items per frame vs adding / removing one or two delegates every handful of frames
		updateID = EngineStructure::Update.Add([=]() { Update(); });
	}
}
// Will disable the object after Update main loop is complete
void GameObject::Disable() {
	GameObject::UnSubscribe();
	// "Bad ID given.  You cannot remove permament delegates (ID of 0)";
	//assert(updateID != 0);
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {
		if(updateID != 0) {
			// This is because many people in the same update loop can tell you "hey, disable yourself"
			// but only the first execution will disable the correct one. I do not have a quick solution to this problem
			// Possibly confirming with a local bool or bitset for state information would be better than constructing a redundant lambda, but then this may be confusing and hard to debug later
			// (Object tests as active half way through the update loop vs Object consistently tests as active the whole update loop and then is disabled with LateUpdate)

			DisableNow();

			//EngineStructure::Update.Remove(updateID);
			//updateID = 0;
		}
	});
}
void GameObject::Update() {

}
void GameObject::UnRender() {
	if (!(flags & RENDERED)) return;
	MessageEvents::SendMessage(EVENT_Unrender, StandardObjectMessage(this));
	flags &= ~RENDERED;
}
void GameObject::Render() {
	if (flags & RENDERED) return;
	MessageEvents::SendMessage(EVENT_Addrender, StandardObjectMessage(this));
	flags |= RENDERED;
}
void GameObject::RenderToFront() {
	if (flags & RENDERED) return;
	MessageEvents::SendMessage(EVENT_Rendertofront, StandardObjectMessage(this));
	flags |= RENDERED;
}
void GameObject::RenderTransparent() {
	if (flags & RENDERED) return;
	MessageEvents::SendMessage(EVENT_Rendertransparent, StandardObjectMessage(this));
	flags |= RENDERED;
}



void GameObject::Destroy() {
	if(!(flags & DESTROYED)) {
		flags |= DESTROYED;
		publicFlags = 0;
		//recycle memory, pool::deactivatememory
		MessageEvents::SendMessage(EVENT_Destroy, StandardObjectMessage(this));
		PersistentUnSubscribe();
		DestroyComponents();
		Components.Clear();
		Disable();
	}
}

void GameObject::OnCollision(GameObject* obj) {}
void GameObject::OnTrigger(GameObject* obj) {}
void GameObject::Awake(Object* obj) {
	componentVariants = obj->componentVariants;
	typeID = 0;
	updateID = 0;
	eventDeleteAllGameObjects = 0;
	flags = RENDERED;
	PersistentSubscribe();
	destroyOnReset = true;
}

//You cannot call this from GameObject::Update
void GameObject::DisableNow() {
	EngineStructure::Update.Remove(updateID);
	updateID = 0;
}




