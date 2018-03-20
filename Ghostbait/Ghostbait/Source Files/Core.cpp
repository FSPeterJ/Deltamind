#include "Core.h"
#include "MessageEvents.h"

void Core::HealedEvent() {
}
void Core::HurtEvent() {
	MessageEvents::SendMessage(EVENT_CoreDamaged, EventMessageBase());
}
void Core::DeathEvent() {
	MessageEvents::SendQueueMessage(EVENT_Late, [=] {Destroy(); });
	MessageEvents::SendMessage(EVENT_GameLose, EventMessageBase());
}
void Core::Destroy() {
	GameObject::Destroy();
}