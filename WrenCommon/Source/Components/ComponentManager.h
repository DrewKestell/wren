#pragma once

#include <EventHandling/Observer.h>
#include <EventHandling/EventHandler.h>
#include <EventHandling/Events/DeleteGameObjectEvent.h>
#include <ObjectManager.h>

template <class T, int maxComponents>
class ComponentManager : public Observer
{
	std::map<int, int> idIndexMap;
	
	void DeleteComponent(const int componentId);

protected:
	EventHandler& eventHandler;
	ObjectManager& objectManager;
	T components[maxComponents];
	int componentIndex{ 0 };

	T& CreateComponent(const int gameObjectId);
public:
	ComponentManager(EventHandler& eventHandler, ObjectManager& objectManager);
	
	T& GetComponentById(const int componentId);
	const bool HandleEvent(const Event* const event) override;
	~ComponentManager();
};

template <class T, int maxComponents>
ComponentManager<T, maxComponents>::ComponentManager(EventHandler& eventHandler, ObjectManager& objectManager)
	: eventHandler{ eventHandler },
	  objectManager{ objectManager }
{
	eventHandler.Subscribe(*this);
}

template <class T, int maxComponents>
T& ComponentManager<T, maxComponents>::CreateComponent(const int gameObjectId)
{
	if (componentIndex == maxComponents)
		throw std::exception("Max Components exceeded!");

	components[componentIndex].id = componentIndex;
	components[componentIndex].gameObjectId = gameObjectId;
	idIndexMap[componentIndex] = componentIndex;

	return components[componentIndex++];
}

template <class T, int maxComponents>
void ComponentManager<T, maxComponents>::DeleteComponent(const int componentId)
{
	// first copy the last Component into the index that was deleted
	const auto componentToDeleteIndex = idIndexMap[componentId];
	const auto lastComponentIndex = --componentIndex;
	memcpy(&components[componentToDeleteIndex], &components[lastComponentIndex], sizeof(T));

	// then update the index of the moved Component
	const auto lastComponentId = components[componentToDeleteIndex].GetId();
	idIndexMap[lastComponentId] = componentToDeleteIndex;
}

template <class T, int maxComponents>
T& ComponentManager<T, maxComponents>::GetComponentById(const int componentId)
{
	const auto index = idIndexMap[componentId];
	return components[index];
}

template <class T, int maxComponents>
const bool ComponentManager<T, maxComponents>::HandleEvent(const Event* const event)
{
	const auto type = event->type;
	switch (type)
	{
		case EventType::DeleteGameObject:
		{
			const auto derivedEvent = (DeleteGameObjectEvent*)event;
			const GameObject& gameObject = objectManager.GetGameObjectById(derivedEvent->gameObjectId);
			DeleteComponent(gameObject.aiComponentId);
			break;
		}
	}
	return false;
}

template <class T, int maxComponents>
ComponentManager<T, maxComponents>::~ComponentManager()
{
	eventHandler.Unsubscribe(*this);
}