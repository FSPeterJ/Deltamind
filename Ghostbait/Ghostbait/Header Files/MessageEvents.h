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
	EVENT_Input,
	EVENT_Instantiate,
};

class EventMessageBase;

class MessageEvents {
private:
	class Delegate: std::function<void(EventMessageBase)> {
		static std::vector<std::function<void(EventMessageBase)>> delegates;
	public:
		void operator()(EventMessageBase e) const {
			for(const std::function<void(EventMessageBase)> element : delegates) {
				element(e);
			}
		}

		inline static void add(const std::function<void(EventMessageBase)> execute) { delegates.push_back(execute); }

		Delegate(void) {};
		~Delegate(void) {};
	};

	static std::unordered_map<EVENT_TYPES, Delegate> eventmap;

	static void HandleMessage(const EVENT_TYPES t, EventMessageBase* m) { HandleMessage(t, *m); }
	static void HandleMessage(const EVENT_TYPES t, EventMessageBase& m);
public:

	/// <summary>
	/// Subscribes to the specified eventtype.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="execute">The function to execute.</param>
	inline static void Subscribe(const EVENT_TYPES eventtype, std::function<void(EventMessageBase)> execute) { eventmap[eventtype].add(execute); };

	/// <summary>
	/// Sends a message and stores it in the event queue.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="message">The message.</param>
	inline static void SendQueueMessage(const EVENT_TYPES eventtype, EventMessageBase* message = 0) { eventmap[eventtype](*message); HandleMessage(eventtype, message); };

	/// <summary>
	/// Sends a message.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="message">The message.</param>
	inline static void SendMessage(EVENT_TYPES eventtype, EventMessageBase& message = EventMessageBase()) { eventmap[eventtype](message); HandleMessage(eventtype, message); };

	//void ProcessEvents();
	MessageEvents();
	~MessageEvents();
};
