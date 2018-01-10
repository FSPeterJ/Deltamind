#pragma once


enum Control {
	none,
	forward,
	backward,
	left,
	right,
	attack,
	teleport,
	menu
};

class EventMessageBase {
public:
	EventMessageBase() {};
	~EventMessageBase() {};
};

class InputMessage: public EventMessageBase {
public:	
	Control c;
	float amount;
 

	 InputMessage() { }

	 void operator=(const EventMessageBase& base) {
		 //EventMessageBase::operator=(base);
	 } 
};
