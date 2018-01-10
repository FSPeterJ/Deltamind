#include "MessageEvents.h"

std::vector<std::function<void()>> MessageEvents::Delegate::delegates;

std::unordered_map<EVENT_TYPES, MessageEvents::Delegate> MessageEvents::eventmap;

MessageEvents::MessageEvents() {}

MessageEvents::~MessageEvents() {}

void MessageEvents::HandleMessage(EVENT_TYPES eventtype, EventMessageBase& m) {
	switch(eventtype) {
	case EVENT_Action:
	{
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
		if(input->GetControl() != none)
		WriteLine(input->GetAmount());
		break;
	}
	default: break;
	}
}
