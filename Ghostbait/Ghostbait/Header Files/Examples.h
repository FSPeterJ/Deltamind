#pragma once

#pragma region Subscribing to Events
#include "MessageEvents.h"
#include "MessageStructs.h"
void FunctionToExecute(EventMessageBase);

void Subscribe(void) {
	MessageEvents::Subscribe(eventType, FunctionToExecute);
}
void FunctionToExecute(EventMessageBase e) {
	WriteLine("FunctionToExecute has been subscribed under eventType");
}
//auto f = [] (int a, int b) -> int { return a + b; };
//int j = (printf("Assigning variable j\n"), 100);
//2[a] = 9
#pragma endregion

#pragma region Sending Messages

void SendMessge(void) {
	MessageEvents::SendMessage(eventType); //Prints the WriteLine above
}
#pragma endregion

#pragma region Object Instantiation

void CreateObject(void) {
	MessageEvents::SendMessage(EVENT_InstantiateRequest, ObjectMessage(0));
}

#pragma endregion
