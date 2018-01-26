#include "MessageEvents.h"

std::unordered_map<EVENT_TYPES, Delegate<EventMessageBase*>> MessageEvents::eventmap;
std::queue<std::function<void()>> MessageEvents::queuedEvents;

MessageEvents::MessageEvents() {
	EngineStructure::LateUpdate += [=]() { ProcessEvents(); };
}

MessageEvents::~MessageEvents() {}

void MessageEvents::ProcessEvents() {
	while(!queuedEvents.empty()) {
		auto f = queuedEvents.front();
		f();
		queuedEvents.pop();
	}
}

//for testing only. used to test messages that have no subscribers yetx
void MessageEvents::HandleMessage(EVENT_TYPES eventtype, EventMessageBase& m) {
	switch(eventtype) {
	case EVENT_InstantiateRequest:
	{
		InstantiateMessage* instantiate = (InstantiateMessage*) &m;
		Console::WriteLine("An object was instantiated with a prefab ID of " + std::to_string(instantiate->GetPrefabId()));
		break;
	}
	case EVENT_LENGTH:
	{
		break;
	}
	case EVENT_Input:
	{
		//InputMessage* input = (InputMessage*) &m;
		//
		//if(input->GetControl() != none) {
		//	Write(input->GetControl());
		//	Write("\t");
		//	WriteLine(input->GetAmount());
		//}

		break;
	}
	default: break;
	}
}
