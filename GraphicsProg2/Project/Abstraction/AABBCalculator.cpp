#include "AABBCalculator.h"
#include <numeric>

std::vector<AABB> AABBCalculator::CalculateAABBs(const std::vector<Vertex>& meshVertices, int amountOfAABBs)
{
    if (meshVertices.size() == 0) return {};

    AABB encompassingAABB{ GetAABBFromVertices(meshVertices) };

    // Create Clusters
    auto clusters{ ClusterVertices(meshVertices, amountOfAABBs, 25, encompassingAABB) };
    std::erase_if(clusters, [&](std::vector<glm::vec3> v)
        {
            return v.size() == 0;
        });

    // Place clusters inside AABBs
    std::vector<AABB> aabbs{ clusters.size() };
    for (int index{}; index < clusters.size(); ++index)
    {
        aabbs[index] = GetAABBFromVertices(clusters[index]);
    }
    return aabbs;
}

std::vector<std::vector<glm::vec3>> AABBCalculator::ClusterVertices(const std::vector<Vertex>& meshVertices, int clusterAmount, int iterations, AABB encompassingAABB)
{
    std::vector<glm::vec3> centers{};
    std::vector<std::vector<glm::vec3>> clusters{};

    centers.resize(clusterAmount);
    clusters.resize(clusterAmount);

    for (auto& center : centers)
        RandomizeVector(center, encompassingAABB);

    for (int iteration{}; iteration < iterations; ++iteration)
    {
        for (const auto& vertex : meshVertices)
        {
            AssignVertexToCluster(vertex.pos, centers, clusters);
        }
        for (int index{}; index < centers.size(); ++index)
        {
            centers[index] = AveragePositionOfVectors(clusters[index]);
            if (centers[index].length() == 0.f) RandomizeVector(centers[index], encompassingAABB);
        }
    }
    return clusters;
}

void AABBCalculator::RandomizeVector(glm::vec3& vector, AABB boundary)
{
    vector.x = (float(rand()) / RAND_MAX) * (boundary.max.x - boundary.min.x) + boundary.min.x;
    vector.y = (float(rand()) / RAND_MAX) * (boundary.max.y - boundary.min.y) + boundary.min.y;
    vector.z = (float(rand()) / RAND_MAX) * (boundary.max.z - boundary.min.z) + boundary.min.z;
}

void AABBCalculator::AssignVertexToCluster(const glm::vec3& vertex, const std::vector<glm::vec3>& centers, std::vector<std::vector<glm::vec3>>& clusters)
{
    int currentClosestIndex{ -1 };
    float currentClosestDistance{ FLT_MAX };

    for (int index{}; index < centers.size(); ++index)
    {
        float distance{ DistanceSquared(centers[index], vertex) };
        if (distance < currentClosestDistance)
        {
            currentClosestIndex = index;
            currentClosestDistance = distance;
        }
    }

    clusters[currentClosestIndex].emplace_back(vertex);
}

glm::vec3 AABBCalculator::AveragePositionOfVectors(std::vector<glm::vec3> positions)
{
    auto v = std::accumulate(positions.begin(), positions.end(), glm::vec3{});
    return v / float(positions.size());
}

float AABBCalculator::DistanceSquared(glm::vec3 vec1, glm::vec3 vec2)
{
    return (vec1.x - vec2.x) * (vec1.x - vec2.x)
        + (vec1.y - vec2.y) * (vec1.y - vec2.y)
        + (vec1.z - vec2.z) * (vec1.z - vec2.z);
}

glm::vec3 AABBCalculator::MinVec(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::vec3{
       std::min(v1.x, v2.x),
       std::min(v1.y, v2.y),
       std::min(v1.z, v2.z)
    };
}

glm::vec3 AABBCalculator::MaxVec(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::vec3{
     std::max(v1.x, v2.x),
     std::max(v1.y, v2.y),
     std::max(v1.z, v2.z)
    };
}

AABB AABBCalculator::GetAABBFromVertices(const std::vector<glm::vec3> vertices)
{
    AABB aabb{};

    aabb.min = vertices[0];
    aabb.max = vertices[0];
    for (const auto& vertex : vertices)
    {
        aabb.min = MinVec(vertex, aabb.min);
        aabb.max = MaxVec(vertex, aabb.max);
    }
    return aabb;
}

AABB AABBCalculator::GetAABBFromVertices(const std::vector<Vertex> vertices)
{
    std::vector<glm::vec3> points{};
    for (const auto& vertex : vertices)
        points.emplace_back(vertex.pos);

    return GetAABBFromVertices(points);
}
