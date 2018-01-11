#pragma once


#include "MessageEvents.h"
void Death(EventMessageBase);
void SendMessge(void) {
	MessageEvents::Subscribe(EVENT_Player_Death, Death);
	MessageEvents::SendMessage(EVENT_Player_Death);
}
void Death(EventMessageBase e) {
	WriteLine("Death");
}
