#pragma once
#include <unordered_map>
#include "MessageStructs.h"
#include "Console.h"
#include "Delegate.h"
#include <queue>
#include "EngineStructure.h"

#undef SendMessage

enum EVENT_TYPES {
	EVENT_Input,
	EVENT_Instantiated,
	EVENT_InstantiateRequest,
	EVENT_InstantiateRequestByType,
	EVENT_Destroy,
	EVENT_Unrender,
	EVENT_Addrender,
	EVENT_LENGTH,
};

class EventMessageBase;

class MessageEvents {
private:
	static std::unordered_map<EVENT_TYPES, Delegate<EventMessageBase*>> eventmap;
	
	static std::queue<std::function<void()>> queuedEvents;

	static void HandleMessage(const EVENT_TYPES t, EventMessageBase* m) { HandleMessage(t, *m); }
	static void HandleMessage(const EVENT_TYPES t, EventMessageBase& m);
public:

	/// <summary>
	/// Subscribes to the specified eventtype.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="execute">The function to execute.</param>
	/// <param name="priority">The optional priority. Leaving it as -1 means no priority. Zero is highest priority.</param>
	inline static void Subscribe(const EVENT_TYPES eventtype, std::function<void(EventMessageBase *)> execute, const int priority = -1) {
		//if(priority < 0) {
		eventmap[eventtype] += execute;
		//return;
	//}

	//size_t index = priority > eventmap.size() - 1 ? eventmap.size() - 1 : priority;
	//eventmap[eventtype].insert(execute, index);
	}

	/// <summary>
	/// Sends a message and stores it in the event queue.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="message">The message.</param>
	template <typename ...T>
	inline static void SendQueueMessage(const EVENT_TYPES eventtype, std::function<void(T...)> execute) {
		
		//execute(T...);
		queuedEvents.push([=]() {execute(T...)});
		//HandleMessage(eventtype, message);
	
	}

	/// <summary>
	/// Sends a message.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="message">The message.</param>
	inline static void SendMessage(const EVENT_TYPES eventtype, EventMessageBase& message = EventMessageBase()) { eventmap[eventtype](&message); HandleMessage(eventtype, message); }

	void ProcessEvents();
	MessageEvents();
	~MessageEvents();
};
