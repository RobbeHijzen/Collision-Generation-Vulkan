#include "CollisionComponent.h"

CollisionComponent::CollisionComponent(Mesh* pParent, bool isStaticMesh)
    : BaseComponent(pParent)
    , m_HasStaticCollision{ isStaticMesh }
{
    Observer* obs{ new Observer(GameEvents::ModelMatrixChanged, [&] { this->CalculateTransformedAABB(); }) };
    pParent->AddObserver(obs);
}

void CollisionComponent::GameStart()
{
    CalculateAABB();
    CalculateTransformedAABB();
}

void CollisionComponent::CalculateAABB()
{
    auto vertices{GetOwner()->GetVertices()};

    if (vertices.size() > 0)
    {
        m_MinAABB = vertices[0].pos;
        m_MaxAABB = vertices[0].pos;
        for (const auto& vertex : vertices)
        {
            m_MinAABB = MinVec(vertex.pos, m_MinAABB);
            m_MaxAABB = MaxVec(vertex.pos, m_MaxAABB);
        }
    }
}

void CollisionComponent::CalculateTransformedAABB()
{
    // This transforms the 8 vertices according to the transform of the triangleMesh
    auto modelMatrix{GetOwner()->GetModelMatrix()};

    // Begin
    glm::vec3 tMinAABB = modelMatrix * glm::vec4{ m_MinAABB, 1.f };
    glm::vec3 tMaxAABB = tMinAABB;

    // (xMax, yMin, zMin)
    glm::vec3 tAABB = modelMatrix * glm::vec4{ m_MaxAABB.x, m_MinAABB.y, m_MinAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMax, yMin, zMax)
    tAABB = modelMatrix * glm::vec4{ m_MaxAABB.x, m_MinAABB.y, m_MaxAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMin, yMin, zMax)
    tAABB = modelMatrix * glm::vec4{ m_MinAABB.x, m_MinAABB.y, m_MaxAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMin, yMax, zMin)
    tAABB = modelMatrix * glm::vec4{ m_MinAABB.x, m_MaxAABB.y, m_MinAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMax, yMax, zMin)
    tAABB = modelMatrix * glm::vec4{ m_MaxAABB.x, m_MaxAABB.y, m_MinAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMax, yMax, zMax)
    tAABB = modelMatrix * glm::vec4{ m_MaxAABB.x, m_MaxAABB.y, m_MaxAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMin, yMax, zMax)
    tAABB = modelMatrix * glm::vec4{ m_MinAABB.x, m_MaxAABB.y, m_MaxAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);

    m_TransformedMinAABB = tMinAABB;
    m_TransformedMaxAABB = tMaxAABB;

    //std::cout << "MinAABB.z: " << m_TransformedMinAABB.z << "   MaxAABB.z: " << m_TransformedMaxAABB.z << "\n";
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
