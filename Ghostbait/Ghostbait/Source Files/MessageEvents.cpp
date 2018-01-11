#include "MessageEvents.h"

std::vector<std::function<void(EventMessageBase*)>> MessageEvents::Delegate::delegates;

std::unordered_map<EVENT_TYPES, MessageEvents::Delegate> MessageEvents::eventmap;

MessageEvents::MessageEvents() {}

MessageEvents::~MessageEvents() {}

//for testing only. used to test messages that have no subscribers yet
void MessageEvents::HandleMessage(EVENT_TYPES eventtype, EventMessageBase& m) {
	switch(eventtype) {
	case EVENT_Action:
	{
		break;
	}
	case EVENT_Instantiate:
	{
		InstantiateMessage* instantiate = (InstantiateMessage*) &m;
		WriteLine("An object was instantiated with a prefab ID of " + std::to_string(instantiate->GetId()));
		break;
	}
	case EVENT_Player_Death:
	{
		break;
	}
	case EVENT_Player_Move:
	{
		break;
	}
	case EVENT_LENGTH:
	{
		break;
	}
	case EVENT_Input:
	{
		InputMessage* input = (InputMessage*) &m;

		if(input->GetControl() != none) {
			Write(input->GetControl());
			Write("\t");
			WriteLine(input->GetAmount());
		}

		break;
	}
	default: break;
	}
}
