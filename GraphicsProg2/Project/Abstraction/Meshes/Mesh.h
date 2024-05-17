#pragma once

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <optional>

#include "Abstraction/VertexInfo.h"
#include "Abstraction/Camera.h"

class Mesh
{
public:
	Mesh(std::string objPath, 
			  std::string diffuseString, 
			  glm::vec3 translation = glm::vec3{0.f, 0.f, 0.f},
			  glm::vec3 rotation = glm::vec3{0.f, 0.f, 0.f},
			  glm::vec3 scale = glm::vec3{1.f, 1.f, 1.f});

	void Draw(VkCommandBuffer buffer) const;

	virtual void GameStart() {};
	virtual void Update(float deltaTime, GLFWwindow* window) {};

	glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }


	const std::vector<uint32_t>& GetIndices() const { return m_Indices; };
	const std::vector<Vertex>& GetVertices() const { return m_Vertices; }

	uint32_t GetMeshIndex() const { return m_MeshIndex.value(); }
	std::string GetDiffuseString() const { return m_DiffuseString; }

	void SetMeshIndex(uint32_t index) { m_MeshIndex = index; }

	virtual void Translate(glm::vec3 addedPos);
	virtual void Rotate(glm::vec3 addedRot);
	virtual void Scale(glm::vec3 addedScale);

	bool IsStaticMesh() const { return m_IsStaticMesh; }

	glm::vec3 GetMinAABB() const { return m_TransformedMinAABB; }
	glm::vec3 GetMaxAABB() const { return m_TransformedMaxAABB; }

protected:

	bool m_IsStaticMesh{ true };

	std::vector<uint32_t> m_Indices{};
	std::vector<Vertex> m_Vertices{};

	glm::mat4 m_ModelMatrix{ glm::mat4{1.f} };

	glm::vec3 m_WorldPos{};
	glm::vec3 m_BaseRot{};
	glm::vec3 m_WorldRot{};
	glm::vec3 m_WorldScale{};

	glm::mat4 m_TranslationMatrix{};
	glm::mat4 m_RotationMatrix{};
	glm::mat4 m_ScaleMatrix{};

	void CalculateWorldMatrix();
	

	std::optional<uint32_t> m_MeshIndex{};
	std::string m_DiffuseString{};


	glm::vec3 m_MinAABB;
	glm::vec3 m_MaxAABB;

	glm::vec3 m_TransformedMinAABB;
	glm::vec3 m_TransformedMaxAABB;

	void CalculateAABB();
	void CalculateTransformedAABB();

	glm::vec3 MinVec(const glm::vec3& v1, const glm::vec3& v2);
	glm::vec3 MaxVec(const glm::vec3& v1, const glm::vec3& v2);
};