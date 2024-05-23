#pragma once

#include "Abstraction/Components/BaseComponent.h"
#include "Abstraction/Meshes/Mesh.h"
#include "Abstraction/HelperStructs.h"


class CollisionComponent : public BaseComponent
{
public:

	CollisionComponent(Mesh* pParent, bool isStaticMesh);
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

	void CalculateAABBs();
	void CalculateTransformedAABBs();

	std::vector<std::vector<glm::vec3>> ClusterVertices(int clusterAmount, int iterations, AABB encompassingAABB);
	void RandomizeVector(glm::vec3& vector, AABB boundary);
	void AssignVertexToCluster(const glm::vec3& vertex, const std::vector<glm::vec3>& centers, std::vector<std::vector<glm::vec3>>& clusters);

	glm::vec3 AveragePositionOfVectors(std::vector<glm::vec3> positions);
	float DistanceSquared(glm::vec3 vec1, glm::vec3 vec2);

	glm::vec3 MinVec(const glm::vec3& v1, const glm::vec3& v2);
	glm::vec3 MaxVec(const glm::vec3& v1, const glm::vec3& v2);

	AABB GetAABBFromVertices(const std::vector<glm::vec3> vertices);
	AABB GetAABBFromVertices(const std::vector<Vertex> vertices);

	static std::vector<Vertex> m_Vertices;
	static std::vector<uint32_t> m_Indices;
};
