#include "MessageEvents.h"




void MessageEvents::Subscribe(const EVENT_TYPES eventtype, const std::function<void()> execute)
{
	eventmap[eventtype].add(execute);
}

void MessageEvents::SendMessage(const EVENT_TYPES eventtype, EventBase* message)
{
	eventmap[eventtype]();
};

MessageEvents::MessageEvents()
{

}


MessageEvents::~MessageEvents()
{
}
