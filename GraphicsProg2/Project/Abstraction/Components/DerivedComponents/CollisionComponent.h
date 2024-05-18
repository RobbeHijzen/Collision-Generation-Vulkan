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

	const std::vector<uint32_t>& GetIndices() const { return m_Indices; };
	const std::vector<std::vector<Vertex>>& GetVertices() const { return m_Vertices; }

	void Render(VkCommandBuffer buffer) const;

private:
	bool m_HasStaticCollision;

	std::vector<AABB> m_AABBs{};
	std::vector<AABB> m_TransformedAABBs{};


	void CalculateAABBs();
	void CalculateTransformedAABBs();

	std::vector<std::vector<glm::vec3>> ClusterVertices(int clusterAmount, int iterations, AABB encompassingAABB);
	void RandomizeVectors(std::vector<glm::vec3>& vectors, AABB boundary);
	void AssignVertexToCluster(const glm::vec3& vertex, const std::vector<glm::vec3>& centers, std::vector<std::vector<glm::vec3>>& clusters);

	glm::vec3 AveragePositionOfVectors(std::vector<glm::vec3> positions);
	float DistanceSquared(glm::vec3 vec1, glm::vec3 vec2);

	glm::vec3 MinVec(const glm::vec3& v1, const glm::vec3& v2);
	glm::vec3 MaxVec(const glm::vec3& v1, const glm::vec3& v2);


	std::vector<std::vector<Vertex>> m_Vertices{};
	std::vector<uint32_t> m_Indices{0, 1, 2,  1, 2, 3,  0, 4, 6,  6, 0, 2,  1, 5, 7,  7, 1, 3,  4, 5, 6,  6, 7, 2};

	void FillVertices();

};
