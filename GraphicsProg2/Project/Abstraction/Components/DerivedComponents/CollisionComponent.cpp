#include "CollisionComponent.h"

#include <numeric>

CollisionComponent::CollisionComponent(Mesh* pParent, bool isStaticMesh)
    : BaseComponent(pParent)
    , m_HasStaticCollision{ isStaticMesh }
{
    Observer* obs{ new Observer(GameEvents::ModelMatrixChanged, [&] { this->CalculateTransformedAABBs(); }) };
    pParent->AddObserver(obs);

    CalculateAABBs();
    FillVertices();
}

void CollisionComponent::GameStart()
{
    CalculateTransformedAABBs();

}

void CollisionComponent::Render(VkCommandBuffer buffer) const
{
    vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void CollisionComponent::CalculateAABBs()
{
    auto vertices{GetOwner()->GetVertices()};
    if (vertices.size() == 0) return;

    AABB encompassingAABB{};

    encompassingAABB.min = vertices[0].pos;
    encompassingAABB.max = vertices[0].pos;
    for (const auto& vertex : vertices)
    {
        encompassingAABB.min = MinVec(vertex.pos, encompassingAABB.min);
        encompassingAABB.max = MaxVec(vertex.pos, encompassingAABB.max);
    }  

    auto clusters{ ClusterVertices(15, 5, encompassingAABB) };
    std::erase_if(clusters, [&](std::vector<glm::vec3> v)
        {
            return v.size() == 0;
        });

    m_AABBs.resize(clusters.size());

    for (int index{}; index < clusters.size(); ++index)
    {
        AABB aabb{};
        aabb.min = clusters[index][0];
        aabb.max = clusters[index][0];
        for (const auto& point : clusters[index])
        {
            aabb.min = MinVec(point, aabb.min);
            aabb.max = MaxVec(point, aabb.max);
        }

        m_AABBs[index] = aabb;
    }
}

std::vector<std::vector<glm::vec3>> CollisionComponent::ClusterVertices(int clusterAmount, int iterations, AABB encompassingAABB)
{
    std::vector<glm::vec3> centers{};
    std::vector<std::vector<glm::vec3>> clusters{};

    auto meshVertices{ GetOwner()->GetVertices() };

    centers.resize(clusterAmount);
    clusters.resize(clusterAmount);

    RandomizeVectors(centers, encompassingAABB);

    for (int iteration{}; iteration < iterations; ++iteration)
    {
        for (const auto& vertex : meshVertices)
        {
            AssignVertexToCluster(vertex.pos, centers, clusters);
        }
        for (int index{}; index < centers.size(); ++index)
        {
            centers[index] = AveragePositionOfVectors(clusters[index]);
        }
    }
    return clusters;
}
void CollisionComponent::RandomizeVectors(std::vector<glm::vec3>& vectors, AABB boundary)
{
    for (auto& vector : vectors)
    {
        vector.x = (float(rand()) / RAND_MAX) * (boundary.max.x - boundary.min.x) + boundary.min.x;
        vector.y = (float(rand()) / RAND_MAX) * (boundary.max.y - boundary.min.y) + boundary.min.y;
        vector.z = (float(rand()) / RAND_MAX) * (boundary.max.z - boundary.min.z) + boundary.min.z;
    }
}
void CollisionComponent::AssignVertexToCluster(const glm::vec3& vertex, const std::vector<glm::vec3>& centers, std::vector<std::vector<glm::vec3>>& clusters)
{
    int currentClosestIndex{ 0 };
    float currentClosestDistance{ FLT_MAX };

    for (int index{ 1 }; index < centers.size(); ++index)
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

glm::vec3 CollisionComponent::AveragePositionOfVectors(std::vector<glm::vec3> positions)
{
    auto v = std::accumulate(positions.begin(), positions.end(), glm::vec3{});
    return v / float(positions.size());
}
float CollisionComponent::DistanceSquared(glm::vec3 vec1, glm::vec3 vec2)
{
    return (vec1.x - vec2.x) * (vec1.x - vec2.x)
         + (vec1.y - vec2.y) * (vec1.y - vec2.y)
         + (vec1.z - vec2.z) * (vec1.z - vec2.z);
}


void CollisionComponent::CalculateTransformedAABBs()
{
    // This transforms the 8 vertices according to the transform of the triangleMesh
    auto modelMatrix{GetOwner()->GetModelMatrix()};

    m_TransformedAABBs.resize(m_AABBs.size());

    for (int i{}; i < m_AABBs.size(); ++i)
    {
        glm::vec3 min{ m_AABBs[i].min};
        glm::vec3 max{ m_AABBs[i].max};

        // Begin
        glm::vec3 tMinAABB = modelMatrix * glm::vec4{ min, 1.f };
        glm::vec3 tMaxAABB = tMinAABB;

        // (xMax, yMin, zMin)
        glm::vec3 tAABB = modelMatrix * glm::vec4{ max.x, min.y, min.z, 1.f };
        tMinAABB = MinVec(tAABB, tMinAABB);
        tMaxAABB = MaxVec(tAABB, tMaxAABB);
        // (xMax, yMin, zMax)
        tAABB = modelMatrix * glm::vec4{ max.x, min.y, max.z, 1.f };
        tMinAABB = MinVec(tAABB, tMinAABB);
        tMaxAABB = MaxVec(tAABB, tMaxAABB);
        // (xMin, yMin, zMax)
        tAABB = modelMatrix * glm::vec4{ min.x, min.y, max.z, 1.f };
        tMinAABB = MinVec(tAABB, tMinAABB);
        tMaxAABB = MaxVec(tAABB, tMaxAABB);
        // (xMin, yMax, zMin)
        tAABB = modelMatrix * glm::vec4{ min.x, max.y, min.z, 1.f };
        tMinAABB = MinVec(tAABB, tMinAABB);
        tMaxAABB = MaxVec(tAABB, tMaxAABB);
        // (xMax, yMax, zMin)
        tAABB = modelMatrix * glm::vec4{ max.x, max.y, min.z, 1.f };
        tMinAABB = MinVec(tAABB, tMinAABB);
        tMaxAABB = MaxVec(tAABB, tMaxAABB);
        // (xMax, yMax, zMax)
        tAABB = modelMatrix * glm::vec4{ max.x, max.y, max.z, 1.f };
        tMinAABB = MinVec(tAABB, tMinAABB);
        tMaxAABB = MaxVec(tAABB, tMaxAABB);
        // (xMin, yMax, zMax)
        tAABB = modelMatrix * glm::vec4{ min.x, max.y, max.z, 1.f };
        tMinAABB = MinVec(tAABB, tMinAABB);
        tMaxAABB = MaxVec(tAABB, tMaxAABB);

        m_TransformedAABBs[i].min = tMinAABB;
        m_TransformedAABBs[i].max = tMaxAABB;
    }
}

glm::vec3 CollisionComponent::MinVec(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::vec3{
       std::min(v1.x, v2.x),
       std::min(v1.y, v2.y),
       std::min(v1.z, v2.z)
    };
}
glm::vec3 CollisionComponent::MaxVec(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::vec3{
      std::max(v1.x, v2.x),
      std::max(v1.y, v2.y),
      std::max(v1.z, v2.z)
    };
}

void CollisionComponent::FillVertices()
{
    m_Vertices.resize(m_AABBs.size());
    for (int index{}; index < m_AABBs.size(); ++index)
    {
        std::vector<glm::vec3> verticesPos{};

        verticesPos.emplace_back(glm::vec3{ m_AABBs[index].min.x, m_AABBs[index].min.y, m_AABBs[index].min.z });
        verticesPos.emplace_back(glm::vec3{ m_AABBs[index].min.x, m_AABBs[index].min.y, m_AABBs[index].max.z });
        verticesPos.emplace_back(glm::vec3{ m_AABBs[index].min.x, m_AABBs[index].max.y, m_AABBs[index].min.z });
        verticesPos.emplace_back(glm::vec3{ m_AABBs[index].min.x, m_AABBs[index].max.y, m_AABBs[index].max.z });
        verticesPos.emplace_back(glm::vec3{ m_AABBs[index].max.x, m_AABBs[index].min.y, m_AABBs[index].min.z });
        verticesPos.emplace_back(glm::vec3{ m_AABBs[index].max.x, m_AABBs[index].min.y, m_AABBs[index].max.z });
        verticesPos.emplace_back(glm::vec3{ m_AABBs[index].max.x, m_AABBs[index].max.y, m_AABBs[index].min.z });
        verticesPos.emplace_back(glm::vec3{ m_AABBs[index].max.x, m_AABBs[index].max.y, m_AABBs[index].max.z });

        for (auto& pos : verticesPos)
        {
            m_Vertices[index].emplace_back(Vertex{pos, {0.f, 0.f}, {0.f, 1.f, 0.f}});
        }
    }
}
