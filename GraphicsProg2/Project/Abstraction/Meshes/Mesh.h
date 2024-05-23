#pragma once

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <optional>

#include "Abstraction/VertexInfo.h"
#include "Abstraction/Camera.h"
#include "Abstraction/EventSystem/Events.h"
#include "Abstraction/EventSystem/Observer.h"

#include "Abstraction/Components/BaseComponent.h"

class Mesh final
{
public:
	Mesh(std::string objPath, 
			  std::string diffuseString, 
			  glm::vec3 translation = glm::vec3{0.f, 0.f, 0.f},
			  glm::vec3 rotation = glm::vec3{0.f, 0.f, 0.f},
			  glm::vec3 scale = glm::vec3{1.f, 1.f, 1.f});

	void Render(VkCommandBuffer buffer) const;

	void GameStart();
	void Update(GLFWwindow* window);
	void LateUpdate();

	glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }
	glm::mat4 GetTranslationMatrix() const { return m_TranslationMatrix; }

	const std::vector<uint32_t>& GetIndices() const { return m_Indices; };
	const std::vector<Vertex>& GetVertices() const { return m_Vertices; }

	uint32_t GetMeshIndex() const { return m_MeshIndex.value(); }
	std::string GetDiffuseString() const { return m_DiffuseString; }

	void SetMeshIndex(uint32_t index) { m_MeshIndex = index; }

	void Rotate(glm::vec3 addedRot);
	void Scale(glm::vec3 addedScale);

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

	glm::vec3 GetWorldPosition() const { return m_WorldPos; }
	glm::vec3 GetWorldRotation() const { return m_WorldRot; }

	void SetVelocity(glm::vec3 inputVelocity) { m_Velocity = inputVelocity; }

private:

	std::vector<uint32_t> m_Indices{};
	std::vector<Vertex> m_Vertices{};

	std::optional<uint32_t> m_MeshIndex{};
	std::string m_DiffuseString{};

	//----------
	// Transformation Handling
	//----------
	glm::mat4 m_ModelMatrix{ glm::mat4{1.f} };

	glm::vec3 m_WorldPos{};
	glm::vec3 m_BaseRot{};
	glm::vec3 m_WorldRot{};
	glm::vec3 m_WorldScale{};

	glm::mat4 m_TranslationMatrix{};
	glm::mat4 m_RotationMatrix{};
	glm::mat4 m_ScaleMatrix{};

	void CalculateWorldMatrix();
	//-----------

	//-----------
	// Movement Handling
	//-----------
	glm::vec3 m_Velocity{};

public:
	void Translate(glm::vec3 addedPos);
private:

	// Components
	std::vector<std::shared_ptr<BaseComponent>> m_Components{};

	// Observers
	std::vector<std::shared_ptr<Observer>> m_Observers{};
};