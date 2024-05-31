#pragma once

#include "HelperStructs.h"
#include "VertexInfo.h"
#include <vector>

class AABBCalculator
{
public:

	static std::vector<AABB> CalculateAABBs(const std::vector<Vertex>& meshVertices, int amountOfAABBs);
	
	static std::vector<std::vector<glm::vec3>> ClusterVertices(const std::vector<Vertex>& meshVertices, int clusterAmount, int iterations, AABB encompassingAABB);
	static void RandomizeVector(glm::vec3& vector, AABB boundary);
	static void AssignVertexToCluster(const glm::vec3& vertex, const std::vector<glm::vec3>& centers, std::vector<std::vector<glm::vec3>>& clusters);
	
	static glm::vec3 AveragePositionOfVectors(std::vector<glm::vec3> positions);
	static float DistanceSquared(glm::vec3 vec1, glm::vec3 vec2);

	static glm::vec3 MinVec(const glm::vec3& v1, const glm::vec3& v2);
	static glm::vec3 MaxVec(const glm::vec3& v1, const glm::vec3& v2);

	static AABB GetAABBFromVertices(const std::vector<glm::vec3> vertices);
	static AABB GetAABBFromVertices(const std::vector<Vertex> vertices);

};