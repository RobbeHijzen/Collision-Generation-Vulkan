#include "AABBCalculator.h"
#include <numeric>
#include <algorithm>
#include <ranges>
#include <execution>

std::vector<AABB> AABBCalculator::CalculateAABBs(const std::vector<Vertex>& meshVertices, const std::vector<uint32_t>& meshIndices, int aabbDepth)
{
    if (meshVertices.size() == 0) return {};
    if (aabbDepth == 0) return { GetAABBFromVertices(meshVertices) };

    std::vector<AABB> partitionedAABBs{};
    partitionedAABBs = GetPartitionedAABBs(GetAABBFromVertices(meshVertices), aabbDepth);

    // sort AABBs
    std::ranges::sort(partitionedAABBs, [&](const AABB& a, const AABB& b) -> bool
        {
            if (a.min.x == b.min.x)
            {
                if (a.min.y == b.min.y)
                {
                    return a.min.z < b.min.z;
                }
                return a.min.y < b.min.y;
            }
            return a.min.x < b.min.x;
        });

    std::vector<AABBVertex> aabbVertices{};
    auto aabbPairs = CreateExtraNeededVertices(PartitionVerticesToAABBs(aabbVertices, meshVertices, partitionedAABBs), aabbVertices, meshIndices, partitionedAABBs);

    RemoveAABBsWithoutVertices(aabbPairs);
    RemakeAABBsBasedOnVertices(aabbPairs);

    std::vector<AABB> finalAABBs{};
    for (const auto& aabbPair : aabbPairs)
        finalAABBs.emplace_back(aabbPair.first);

    return finalAABBs;
}

std::vector<AABB> AABBCalculator::GetPartitionedAABBs(const AABB& encompassingAABB, int depthRemaining)
{
    std::vector<AABB> smallerAABBs{};
    GetSmallerAABBs(smallerAABBs, encompassingAABB);

    if (depthRemaining == 1)
    {
        return smallerAABBs;
    }
    else
    {
        std::vector<AABB> returningAABBs{};

        for (const auto& smallerAABB : smallerAABBs)
        {
            for (const auto& finalAABB : GetPartitionedAABBs(smallerAABB, depthRemaining - 1))
            {
                returningAABBs.emplace_back(finalAABB);
            }
        }

        return returningAABBs;
    }

    return std::vector<AABB>();
}

std::vector<AABBVertexInfo> AABBCalculator::PartitionVerticesToAABBs(std::vector<AABBVertex>& aabbVertices, const std::vector<Vertex>& vertices, const std::vector<AABB>& aabbs)
{
    std::vector<AABBVertexInfo> partitionedAABBs{};
    aabbVertices.resize(vertices.size());
    
    int aabbIndex{};
    for (const auto& aabb : aabbs)
    {
        std::pair<AABB, std::vector<AABBVertex>> pair{};
        pair.first = aabb;
    
        int vertexIndex{};
        for (const auto& vertex : vertices)
        {
            if (vertex.pos.x >= aabb.min.x && vertex.pos.x <= aabb.max.x &&
                vertex.pos.y >= aabb.min.y && vertex.pos.y <= aabb.max.y &&
                vertex.pos.z >= aabb.min.z && vertex.pos.z <= aabb.max.z)
            {
                AABBVertex aabbVertex{ vertex,  aabbIndex };
                pair.second.emplace_back(aabbVertex);
                aabbVertices[vertexIndex] = aabbVertex;
            }

            ++vertexIndex;
        }
    
        partitionedAABBs.emplace_back(pair);
        ++aabbIndex;
    }
    return partitionedAABBs;
}

std::vector<AABBInfo> AABBCalculator::CreateExtraNeededVertices(const std::vector<AABBVertexInfo>& aabbInfos, const std::vector<AABBVertex>& aabbVertices, const std::vector<uint32_t>& meshIndices, const std::vector<AABB>& allAABBs)
{
    std::vector<AABBInfo> result{};
    for (const auto& aabbInfo : aabbInfos)
    {
        std::vector<Vertex> vertices{};
        for (const auto& vertex : aabbInfo.second)
            vertices.emplace_back(vertex.vertex);

        result.emplace_back(std::pair<AABB, std::vector<Vertex>>{aabbInfo.first, vertices});
    }

    for (int index{}; index < meshIndices.size(); index += 3)
    {
        std::vector<AABBVertex> vertices{ aabbVertices[meshIndices[index]], aabbVertices[meshIndices[index + 1]], aabbVertices[meshIndices[index + 2]] };

        // Calculate intersection points on the borders
        for (int i = 0; i < 3; ++i) 
        {
            AABBVertex vA = vertices[i];
            AABBVertex vB = vertices[(i + 1) % 3];

            if (vA.AABBIndex != vB.AABBIndex) 
            {
                auto AABBs{ GetTrianglesFromAABBIndices(allAABBs, vA.AABBIndex, vB.AABBIndex)};
                Ray ray{vA.vertex.pos, vB.vertex.pos - vA.vertex.pos};
                
                for (const auto& aabb : AABBs)
                {
                    for (const auto& triangle : aabb.first)
                    {
                        HitRecord hitRecord{};
                        if (HitTestTriangle(triangle, ray, hitRecord))
                        {
                            result[aabb.second].second.emplace_back(hitRecord.pos);
                        }
                    }
                    
                }
            }
        }
    }

    return result;
}

void AABBCalculator::RemoveAABBsWithoutVertices(std::vector<std::pair<AABB, std::vector<Vertex>>>& aabbs)
{
    aabbs.erase(std::remove_if(aabbs.begin(), aabbs.end(), [](std::pair<AABB, std::vector<Vertex>> aabb)
        {
            return aabb.second.size() == 0;
        }), aabbs.end());
}

void AABBCalculator::RemakeAABBsBasedOnVertices(std::vector<std::pair<AABB, std::vector<Vertex>>>& aabbs)
{
    std::for_each(std::execution::par, aabbs.begin(), aabbs.end(), [&](AABBInfo& aabb)
        {
            aabb.first = GetAABBFromVertices(aabb.second);
        });
}

bool AABBCalculator::DoesAABBHaveVertex(const std::vector<Vertex>& vertices, const AABB& aabb)
{
    for (const auto& vertex : vertices)
    {
        if (vertex.pos.x >= aabb.min.x && vertex.pos.x <= aabb.max.x &&
            vertex.pos.y >= aabb.min.y && vertex.pos.y <= aabb.max.y &&
            vertex.pos.z >= aabb.min.z && vertex.pos.z <= aabb.max.z)
        {
            return true;
        }
    }
    return false;
}

std::vector<AABBCalculator::Triangle> AABBCalculator::GetTriangleFromAABB(const AABB& aabb)
{
    std::vector<Triangle> triangles{12};

    glm::vec3 v0 = { aabb.min.x, aabb.min.y, aabb.min.z };
    glm::vec3 v1 = { aabb.max.x, aabb.min.y, aabb.min.z };
    glm::vec3 v2 = { aabb.max.x, aabb.max.y, aabb.min.z };
    glm::vec3 v3 = { aabb.min.x, aabb.max.y, aabb.min.z };
    glm::vec3 v4 = { aabb.min.x, aabb.min.y, aabb.max.z };
    glm::vec3 v5 = { aabb.max.x, aabb.min.y, aabb.max.z };
    glm::vec3 v6 = { aabb.max.x, aabb.max.y, aabb.max.z };
    glm::vec3 v7 = { aabb.min.x, aabb.max.y, aabb.max.z };

    triangles[0].v0 = v0;
    triangles[0].v1 = v1;
    triangles[0].v2 = v2;

    triangles[1].v0 = v0;
    triangles[1].v1 = v2;
    triangles[1].v2 = v3;

    triangles[2].v0 = v4;
    triangles[2].v1 = v6;
    triangles[2].v2 = v5;

    triangles[3].v0 = v4;
    triangles[3].v1 = v7;
    triangles[3].v2 = v6;

    triangles[4].v0 = v0;
    triangles[4].v1 = v3;
    triangles[4].v2 = v7;

    triangles[5].v0 = v0;
    triangles[5].v1 = v7;
    triangles[5].v2 = v4;

    triangles[6].v0 = v1;
    triangles[6].v1 = v5;
    triangles[6].v2 = v6;

    triangles[7].v0 = v1;
    triangles[7].v1 = v6;
    triangles[7].v2 = v2;

    triangles[8].v0 = v3;
    triangles[8].v1 = v2;
    triangles[8].v2 = v6;

    triangles[9].v0 = v3;
    triangles[9].v1 = v6;
    triangles[9].v2 = v7;

    triangles[10].v0 = v0;
    triangles[10].v1 = v4;
    triangles[10].v2 = v5;

    triangles[11].v0 = v0;
    triangles[11].v1 = v5;
    triangles[11].v2 = v1;

    return triangles;
}

std::vector<std::pair<std::vector<AABBCalculator::Triangle>, int>> AABBCalculator::GetTrianglesFromAABBIndices(std::vector<AABB> allAABBs, int firstIndex, int secondIndex)
{
    std::vector<std::pair<std::vector<AABBCalculator::Triangle>, int>> triangles{};
    int cubeLength{ static_cast<int>(cbrtf(static_cast<float>(allAABBs.size()))) };

    CoordinateIndices firstAABBCoords{ GetCoordinatedFromIndex(firstIndex, cubeLength)};


    CoordinateIndices secondAABBCoords{ GetCoordinatedFromIndex(secondIndex, cubeLength)};

    CoordinateIndices minCoords{ std::min(firstAABBCoords.x, secondAABBCoords.x),
                                 std::min(firstAABBCoords.y, secondAABBCoords.y), 
                                 std::min(firstAABBCoords.z, secondAABBCoords.z) };

    CoordinateIndices maxCoords{ std::max(firstAABBCoords.x, secondAABBCoords.x),
                                 std::max(firstAABBCoords.y, secondAABBCoords.y),
                                 std::max(firstAABBCoords.z, secondAABBCoords.z) };

    auto indices{ GetIndicesFromMinMaxCoordinates(minCoords, maxCoords, cubeLength) };
   
    for (int index : indices)
    {
        triangles.emplace_back(GetTriangleFromAABB(allAABBs[index]), index);
    }

    return triangles;
}

bool AABBCalculator::HitTestTriangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord)
{
    const glm::vec3 edge1{ triangle.v1 - triangle.v0 };
    const glm::vec3 edge2{ triangle.v2 - triangle.v0 };

    const glm::vec3 pVec{ glm::cross(ray.direction, edge2) };
    const float determinant{ glm::dot(edge1, pVec) };

    if (determinant < FLT_EPSILON && determinant > -FLT_EPSILON) return false;

    const float inverseDeterminant{ 1.f / determinant };
    const glm::vec3 tVec{ ray.origin - triangle.v0 };

    const float u{ glm::dot(tVec, pVec) * inverseDeterminant };
    if (u < 0 || u > 1) return false;


    const glm::vec3 qVec{ glm::cross(tVec, edge1) };

    const float v{ glm::dot(ray.direction, qVec) * inverseDeterminant };
    if (v < 0 || u + v > 1) return false;

    const float t{ glm::dot(edge2, qVec) * inverseDeterminant };

    if (t < 0.f || t > ray.max)
    {
        return false;
    }
   
    hitRecord.pos = ray.origin + ray.direction * t;
    return true;
}

AABBCalculator::CoordinateIndices AABBCalculator::GetCoordinatedFromIndex(int index, int cubeLength)
{
    return CoordinateIndices{ index % cubeLength,
                              (index / cubeLength) % cubeLength, 
                              index / (cubeLength * cubeLength)};
}

std::vector<int> AABBCalculator::GetIndicesFromMinMaxCoordinates(CoordinateIndices minCoords, CoordinateIndices maxCoords, int cubeLength)
{
    std::vector<int> indices{};

    for (int xIndex{ minCoords.x }; xIndex <= maxCoords.x; ++xIndex)
    {
        for (int yIndex{ minCoords.y }; yIndex <= maxCoords.y; ++yIndex)
        {
            for (int zIndex{ minCoords.z }; zIndex <= maxCoords.z; ++zIndex)
            {
                indices.emplace_back(xIndex + yIndex * cubeLength + zIndex * cubeLength * cubeLength);
            }
        }
    }

    return indices;
}

void AABBCalculator::GetSmallerAABBs(std::vector<AABB>& smallerAABBs, const AABB& e)
{
    float xSize{ e.max.x - e.min.x };
    float ySize{ e.max.y - e.min.y };
    float zSize{ e.max.z - e.min.z };

    for (int i{}; i < 2; ++i)
    {
        for (int j{}; j < 2; ++j)
        {
            for (int k{}; k < 2; ++k)
            {
                AABB aabb{};

                aabb.min.x = (i == 0) ? e.min.x : e.min.x + xSize / 2;
                aabb.min.y = (j == 0) ? e.min.y : e.min.y + ySize / 2;
                aabb.min.z = (k == 0) ? e.min.z : e.min.z + zSize / 2;

                aabb.max.x = (i == 0) ? e.max.x - xSize / 2 : e.max.x ;
                aabb.max.y = (j == 0) ? e.max.y - ySize / 2 : e.max.y ;
                aabb.max.z = (k == 0) ? e.max.z - zSize / 2 : e.max.z ;

                smallerAABBs.emplace_back(aabb);
            }
        }
    }
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