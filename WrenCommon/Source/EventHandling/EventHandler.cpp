#include "stdafx.h"
#include "EventHandler.h"

void EventHandler::Subscribe(Observer& observer) { observers.push_back(&observer); }
void EventHandler::Unsubscribe(Observer& observer) { observers.remove(&observer); }
void EventHandler::QueueEvent(std::unique_ptr<Event>& event) { eventQueue.push(std::move(event)); }
std::queue<std::unique_ptr<const Event>>& EventHandler::GetEventQueue() { return eventQueue; }
std::list<Observer*>& EventHandler::GetObservers() { return observers; }