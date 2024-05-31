#pragma once

#include "Abstraction/Components/BaseComponent.h"
#include "Abstraction/Meshes/Mesh.h"
#include "Abstraction/HelperStructs.h"


class CollisionComponent : public BaseComponent
{
public:

	CollisionComponent(Mesh* pParent, bool isStaticMesh, int numAABBs);
	virtual void GameStart() override;

	bool HasStaticCollision() const { return m_HasStaticCollision; }

	auto GetAABBs() const { return m_TransformedAABBs; }
	auto GetModelMatrices() const { return m_ModelMatrices; }

	static const std::vector<uint32_t>& GetIndices() { return m_Indices; };
	static const std::vector<Vertex>& GetVertices() { return m_Vertices; }

	void Render(VkCommandBuffer buffer) const;

private:

	std::vector<glm::mat4> m_ModelMatrices{};
	void UpdateModelMatrix();

	bool m_HasStaticCollision;

	std::vector<AABB> m_AABBs{};
	std::vector<AABB> m_TransformedAABBs{};

	void CalculateTransformedAABBs();

	static std::vector<Vertex> m_Vertices;
	static std::vector<uint32_t> m_Indices;
};
