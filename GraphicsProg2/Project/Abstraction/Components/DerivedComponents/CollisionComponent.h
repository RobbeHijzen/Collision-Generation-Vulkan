#pragma once

#include "Abstraction/Components/BaseComponent.h"
#include "Abstraction/Meshes/Mesh.h"


class CollisionComponent : public BaseComponent
{
public:

	CollisionComponent(Mesh* pParent, bool isStaticMesh);
	virtual void GameStart() override;

	bool HasStaticCollision() const { return m_HasStaticCollision; }

	glm::vec3 GetMinAABB() const { return m_TransformedMinAABB; }
	glm::vec3 GetMaxAABB() const { return m_TransformedMaxAABB; }

	const std::vector<uint32_t>& GetIndices() const { return m_Indices; };
	const std::vector<Vertex>& GetVertices() const { return m_Vertices; }

	void Render(VkCommandBuffer buffer) const;

private:
	bool m_HasStaticCollision;

	glm::vec3 m_MinAABB{};
	glm::vec3 m_MaxAABB{};

	glm::vec3 m_TransformedMinAABB{};
	glm::vec3 m_TransformedMaxAABB{};

	void CalculateAABB();
	void CalculateTransformedAABB();

	glm::vec3 MinVec(const glm::vec3& v1, const glm::vec3& v2);
	glm::vec3 MaxVec(const glm::vec3& v1, const glm::vec3& v2);


	std::vector<Vertex> m_Vertices{};
	std::vector<uint32_t> m_Indices{0, 1, 2,  1, 2, 3,  0, 4, 6,  6, 0, 2,  1, 5, 7,  7, 1, 3,  4, 5, 6,  6, 7, 2};

	void FillVertices();

};
