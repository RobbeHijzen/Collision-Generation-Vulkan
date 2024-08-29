#pragma once

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <optional>

#include "VulkanBase/Scene/Object.h"
#include "VulkanBase/HelperStructs/IRenderable.h"

#include "VulkanBase/HelperStructs/VertexInfo.h"
#include "PhysicsGame/Camera.h"
#include "Vulkanbase/EventSystem/Events.h"
#include "Vulkanbase/EventSystem/Observer.h"

#include "Vulkanbase/Helperstructs/BaseComponent.h"

class Mesh final : public Object, public IRenderable
{
public:
	Mesh(std::string objPath, 
		 std::string diffuseString, 
		 glm::vec3 translation = glm::vec3{0.f, 0.f, 0.f},
		 glm::vec3 rotation = glm::vec3{0.f, 0.f, 0.f},
		 glm::vec3 scale = glm::vec3{1.f, 1.f, 1.f});


	void Render(VkCommandBuffer buffer) const override;
	void Update(GLFWwindow* window) override;

	glm::mat4 GetModelMatrix() const override { return m_ModelMatrix; }

	const std::vector<uint32_t>& GetIndices() const override { return m_Indices; };
	const std::vector<Vertex>& GetVertices() const override { return m_Vertices; }

	uint32_t GetRenderID() const override { return m_RenderID.value(); }
	void SetRenderID(uint32_t index) override { m_RenderID = index; }
	PipelinesEnum GetPipelineID() const override { return m_PipelineID; }

	std::string GetDiffuseString() const override { return m_DiffuseString; }

	void Rotate(glm::vec3 addedRot);
	void Scale(glm::vec3 addedScale);

	
	glm::vec3 GetWorldPosition() const { return m_WorldPos; }
	glm::vec3 GetWorldRotation() const { return m_WorldRot; }

	void SetVelocity(glm::vec3 inputVelocity) { m_Velocity = inputVelocity; }

private:

	std::vector<uint32_t> m_Indices{};
	std::vector<Vertex> m_Vertices{};

	std::optional<uint32_t> m_RenderID{};
	std::string m_DiffuseString{};
	PipelinesEnum m_PipelineID{ PipelinesEnum::regular };

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