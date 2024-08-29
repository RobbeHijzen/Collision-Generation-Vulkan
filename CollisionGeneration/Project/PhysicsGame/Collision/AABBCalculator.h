#pragma once

#include "VulkanBase/HelperStructs/HelperStructs.h"
#include "Vulkanbase/HelperStructs/VertexInfo.h"
#include <vector>
#include <set>

struct AABBVertex
{
	Vertex vertex{};
	int AABBIndex{};
};

using AABBInfo = std::pair<AABB, std::vector<Vertex>>;
using AABBVertexInfo = std::pair<AABB, std::vector<AABBVertex>>;

class AABBCalculator
{
public:

	struct Triangle
	{
		glm::vec3 v0{};
		glm::vec3 v1{};
		glm::vec3 v2{};

		glm::vec3 normal{};
	};

	struct Ray
	{
		glm::vec3 origin{};
		glm::vec3 direction{};
		float max{1.f};
	};

	struct HitRecord
	{
		glm::vec3 pos{};
	};

	struct CoordinateIndices
	{
		int x{};
		int y{};
		int z{};
	};

	static std::vector<AABB> CalculateAABBs(const std::vector<Vertex>& meshVertices, const std::vector<uint32_t>& meshIndices,int aabbDepth);
	
	static glm::vec3 AveragePositionOfVectors(std::vector<glm::vec3> positions);
	static float DistanceSquared(glm::vec3 vec1, glm::vec3 vec2);

	static glm::vec3 MinVec(const glm::vec3& v1, const glm::vec3& v2);
	static glm::vec3 MaxVec(const glm::vec3& v1, const glm::vec3& v2);

	static AABB GetAABBFromVertices(const std::vector<glm::vec3> vertices);
	static AABB GetAABBFromVertices(const std::vector<Vertex> vertices);

private:

	static std::vector<AABB> GetPartitionedAABBs(const AABB& encompassingAABB, int depthRemaining);
	static void GetSmallerAABBs(std::vector<AABB>& smallerAABBs, const AABB& encompassingAABB);

	static std::vector<AABBVertexInfo> PartitionVerticesToAABBs(std::vector<AABBVertex>& aabbVertices, const std::vector<Vertex>& vertices, const std::vector<AABB>& aabbs);
	static std::vector<AABBInfo> CreateExtraNeededVertices(const std::vector<AABBVertexInfo>& aabbInfos, const std::vector<AABBVertex>& aabbVertices, const std::vector<uint32_t>& meshIndices, const std::vector<AABB>& allAABBs);
	
	static void RemoveAABBsWithoutVertices(std::vector<AABBInfo>& aabbs);
	static void RemakeAABBsBasedOnVertices(std::vector<AABBInfo>& aabbs);
	static bool DoesAABBHaveVertex(const std::vector<Vertex>& vertices, const AABB& aabb);

	static std::vector<Triangle> GetTriangleFromAABB(const AABB& aabb);
	static std::vector<std::pair<std::vector<AABBCalculator::Triangle>, int>> GetTrianglesFromAABBIndices(std::vector<AABB> allAABBs, int firstIndex, int secondIndex);
	static bool HitTestTriangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord);
	static CoordinateIndices GetCoordinatedFromIndex(int index, int cubeLength);
	static std::vector<int> GetIndicesFromMinMaxCoordinates(CoordinateIndices minCoords, CoordinateIndices maxCoords, int cubeLength);
};