#pragma once
#include <unordered_map>
#include <functional>
#include "MessageStructs.h"
#include "Console.h"
using namespace Console;

enum EVENT_TYPES {
	EVENT_Action,
	EVENT_Player_Death,
	EVENT_Player_Move,
	EVENT_LENGTH,
	EVENT_Input
};

class EventMessageBase;



class MessageEvents {
private:
	class event_func: std::function<void()> {
		static std::vector<std::function<void()>> delegates;
	public:
		void operator()() const {
			for(const std::function<void()> element : delegates) {
				element();
			}
		}
		static void add(const std::function<void()> execute) {
			delegates.push_back(execute);
		}
		event_func() {};
		~event_func() {};
	};

	static std::unordered_map<int, event_func> eventmap;

public:

	void  Subscribe(const EVENT_TYPES eventtype, const std::function<void()>  execute) {
		eventmap[eventtype].add(execute);
	};
	static void SendMessage(const EVENT_TYPES eventtype, EventMessageBase* message) {
		eventmap[eventtype]();
	};
	static void SendMessage(EVENT_TYPES eventtype, EventMessageBase& message) {
		eventmap[eventtype]();

		if(eventtype == EVENT_Input) {
			InputMessage* input = (InputMessage*)&message;
			WriteLine(input->amount);
		}
	};

	//void ProcessEvents();
	MessageEvents();
	~MessageEvents();
};
