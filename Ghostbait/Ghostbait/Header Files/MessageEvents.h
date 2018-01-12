#pragma once
#include <unordered_map>
#include "MessageStructs.h"
#include "Console.h"
#include "Delegate.h"
using namespace Console;

enum EVENT_TYPES {
	EVENT_Input,
	EVENT_InstantiateRequest,
	EVENT_Instantiated,
	EVENT_Destroy,
	EVENT_LENGTH,
};

class EventMessageBase;

class MessageEvents {
private:

	static std::unordered_map<EVENT_TYPES, Delegate<EventMessageBase*>> eventmap;

	static void HandleMessage(const EVENT_TYPES t, EventMessageBase* m) { HandleMessage(t, *m); }
	static void HandleMessage(const EVENT_TYPES t, EventMessageBase& m);
public:

	/// <summary>
	/// Subscribes to the specified eventtype.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="execute">The function to execute.</param>
	inline static void Subscribe(const EVENT_TYPES eventtype, std::function<void(EventMessageBase *)> execute) { eventmap[eventtype].add(execute); };

	/// <summary>
	/// Sends a message and stores it in the event queue.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="message">The message.</param>
	inline static void SendQueueMessage(const EVENT_TYPES eventtype, EventMessageBase* message = 0) { eventmap[eventtype](message); HandleMessage(eventtype, message); };

	/// <summary>
	/// Sends a message.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="message">The message.</param>
	inline static void SendMessage(const EVENT_TYPES eventtype, EventMessageBase& message = EventMessageBase()) { eventmap[eventtype](&message); HandleMessage(eventtype, message); };

	//void ProcessEvents();
	MessageEvents();
	~MessageEvents();
};
