#pragma once

#pragma region Subscribing to Events
#include "MessageEvents.h"
void FunctionToExecute(EventMessageBase);

void Subscribe(void) {
	MessageEvents::Subscribe(eventType, FunctionToExecute);
}
void FunctionToExecute(EventMessageBase e) {
	WriteLine("FunctionToExecute has been subscribed under eventType");
}

#pragma endregion

#pragma region Sending Messages

void SendMessge(void) {
	MessageEvents::SendMessage(eventType); //Prints the WriteLine above
}
#pragma endregion

#pragma region Object Instantiation

void CreateObject(void) {
	MessageEvents::SendMessage(EVENT_InstantiateRequest, NewObjectMessage(0));
}

#pragma endregion
