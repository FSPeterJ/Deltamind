#include "Core.h"
#include "MessageEvents.h"

void Core::Death() {
	Console::WriteLine << "YOU LOSE!";
	Console::OutLine << "YOU LOSE!";
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	MessageEvents::SendMessage(EVENT_GameLose, EventMessageBase());
}
void Core::Destroy() {
	GameObject::Destroy();
}