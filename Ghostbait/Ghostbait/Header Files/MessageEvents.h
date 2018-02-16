#pragma once
#include <unordered_map>
#include "Delegate.h"
#include <queue>

#undef SendMessage

enum EVENT_TYPES {
	EVENT_Input,
	EVENT_Instantiated,
	EVENT_InstantiateRequest,
	EVENT_InstantiateRequestByType,
	EVENT_InstantiateRequestByName_DEBUG_ONLY,
	EVENT_Destroy,
	EVENT_Unrender,
	EVENT_Addrender,
	EVENT_Late,
	EVENT_LENGTH,
	EVENT_RegisterNoisemaker,
	EVENT_RequestSound,
	EVENT_UnregisterNoisemaker,
	
	//Game Events
	EVENT_SpawnerCreated,
	EVENT_EnemyDied,
	EVENT_StartWave,
	EVENT_GameWin,
	EVENT_GameLose,
	EVENT_GamePause,
	EVENT_GameResume,
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
	static unsigned Subscribe(const EVENT_TYPES eventtype, std::function<void(EventMessageBase *)> execute, const int priority = -1);

	/// <summary>
	/// Sends a message and stores it in the event queue.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="message">The message.</param>
	static void SendQueueMessage(const EVENT_TYPES eventtype, std::function<void(void)> execute);
	/// <summary>
	/// Sends a message.
	/// </summary>
	/// <param name="eventtype">The eventtype.</param>
	/// <param name="message">The message.</param>
	static void SendMessage(const EVENT_TYPES eventtype, EventMessageBase& message);

	static void ProcessEvents();

	static void Initilize();
};
