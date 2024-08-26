#pragma once
#include "Vulkanbase/Helperstructs/BaseComponent.h"
#include "Vulkanbase/EventSystem/Events.h"
#include "Vulkanbase/EventSystem/Observer.h"

#include <memory>

class Object 
{
public:
	Object() {}

	virtual void GameStart() 
	{
		for (auto& component : m_Components)
		{
			component->GameStart();
		}
	};
	virtual void Update(GLFWwindow* window)
	{
		for (auto& component : m_Components)
		{
			component->Update(window);
		}
	};
	virtual void LateUpdate() 
	{
		for (auto& component : m_Components)
		{
			component->LateUpdate();
		}
	}

	bool AddComponent(std::shared_ptr<BaseComponent> component)
	{
		if (component.get())
		{
			m_Components.emplace_back(component);
			return true;
		}
		return false;
	}

	template<typename T>
	std::shared_ptr<T> GetComponent()
	{
		for (auto& component : m_Components)
		{
			if (auto castedComponent = std::dynamic_pointer_cast<T>(component))
			{
				return castedComponent;
			}
		}
		return nullptr;
	}

	auto GetAllComponents() const
	{
		std::vector<BaseComponent*> comps{};
		for (const auto& comp : m_Components)
			comps.emplace_back(comp.get());

		return comps;
	}

	void AddObserver(Observer* observer)
	{
		m_Observers.emplace_back(observer);
	}
	void NotifyObservers(GameEvents event)
	{
		for (auto& observer : m_Observers)
		{
			if (observer->event == event)
			{
				observer->function();
			}
		}
	}
private:

	// Components
	std::vector<std::shared_ptr<BaseComponent>> m_Components{};

	// Observers
	std::vector<std::shared_ptr<Observer>> m_Observers{};
};