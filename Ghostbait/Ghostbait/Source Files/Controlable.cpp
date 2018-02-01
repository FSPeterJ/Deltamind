#include "Controlable.h"
#include "MessageEvents.h"  // for EVENT_TYPES::EVENT_Input, MessageEvents

Controlable::Controlable() { MessageEvents::Subscribe(EVENT_Input, inputReceivedEvent.ReceiveInputEvent()); }
Controlable::~Controlable() {}
