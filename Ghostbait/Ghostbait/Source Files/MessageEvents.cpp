#include "MessageEvents.h"

std::vector<std::function<void()>> MessageEvents::event_func::delegates;

std::unordered_map<int, MessageEvents::event_func> MessageEvents::eventmap;

MessageEvents::MessageEvents() {}

MessageEvents::~MessageEvents() {}
