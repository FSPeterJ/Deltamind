#pragma once
#include <unordered_map>
#include <functional>


enum EVENT_TYPES
{
	EVENT_Action,
	EVENT_Player_Death,
	EVENT_Player_Move,
	EVENT_LENGTH
};

class EventBase
{
public:
	EVENT_TYPES type;
	EventBase(EVENT_TYPES _type):type(_type) {};
	~EventBase() {};
};


class MessageEvents
{
private:
	class event_func: std::function<void()>
	{
		static std::vector<std::function<void()>> delegates;
	public:
		void operator()() const
		{
			for(const std::function<void()> element : delegates)
			{
				element();
			}
		}
		void static add(const std::function<void()> execute)
		{
			delegates.push_back(execute);
		}
		event_func() {};
		~event_func() {};

	};
	static std::unordered_map<int, event_func> eventmap;

public:
	void static Subscribe(const EVENT_TYPES eventtype, const std::function<void()>  execute);

	void static SendMessage(const EVENT_TYPES eventtype, EventBase* message);

	//void ProcessEvents();
	MessageEvents();
	~MessageEvents();
};