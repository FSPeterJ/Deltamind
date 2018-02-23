#include "MessageEvents.h"
#include "EngineStructure.h"
#include "MessageStructs.h"
#undef SendMessage

std::unordered_map<EVENT_TYPES, Delegate<EventMessageBase*>> MessageEvents::eventmap;
std::queue<std::function<void()>> MessageEvents::queuedEvents;

void MessageEvents::Initilize() {
	EngineStructure::LateUpdate += [=]() { ProcessEvents(); };
}

unsigned MessageEvents::Subscribe(const EVENT_TYPES eventtype,
	std::function<void(EventMessageBase *)> execute, const int priority) {
	//if(priority < 0) {
	return eventmap[eventtype].Add(execute);
	//return;
	//}

	//size_t index = priority > eventmap.size() - 1 ? eventmap.size() - 1 : priority;
	//eventmap[eventtype].insert(execute, index);
}

void MessageEvents::UnSubscribe(const EVENT_TYPES eventtype, unsigned subscriptionID) {
	eventmap[eventtype].Remove(subscriptionID);
}

void MessageEvents::SendMessage(const EVENT_TYPES eventtype, EventMessageBase& message) {
	eventmap[eventtype](&message); HandleMessage(eventtype, message);
}

void MessageEvents::SendQueueMessage(const EVENT_TYPES eventtype, std::function<void(void)> execute) {
	//execute(T...);
	queuedEvents.push([=]() {
		execute();
	});
	//HandleMessage(eventtype, message);
}

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
		Console::WriteLine << "An object was instantiated with a prefab ID of " << instantiate->GetPrefabId();
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
