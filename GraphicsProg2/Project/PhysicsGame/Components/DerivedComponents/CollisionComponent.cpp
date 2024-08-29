#include "CollisionComponent.h"
#include "PhysicsGame/MathHelpers.h"
#include "PhysicsGame/Collision/AABBCalculator.h"

#include <numeric>
#include <fstream>

CollisionComponent::CollisionComponent(Mesh* pParent, bool isStaticMesh, int aabbDepth)
    : BaseComponent(pParent)
    , m_HasStaticCollision{ isStaticMesh }
{
    Observer* obs{ new Observer(GameEvents::ModelMatrixChanged, [&] { this->CalculateTransformedAABBs(); }) };
    pParent->AddObserver(obs);
    obs = new Observer(GameEvents::ModelMatrixChanged, [&] { this->UpdateModelMatrix(); });
    pParent->AddObserver(obs);

    m_AABBs = AABBCalculator::CalculateAABBs(GetOwner()->GetVertices(), GetOwner()->GetIndices(), aabbDepth);
    CalculateTransformedAABBs();

    m_ModelMatrices.resize(m_AABBs.size());
}

void CollisionComponent::GameStart()
{
    CalculateTransformedAABBs();
    UpdateModelMatrix();
}

void CollisionComponent::Render(VkCommandBuffer buffer) const
{
    vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void CollisionComponent::UpdateModelMatrix()
{    
    for (int index{}; index < m_ModelMatrices.size(); ++index)
    {
        glm::vec3 scale{ m_TransformedAABBs[index].max - m_TransformedAABBs[index].min};
        glm::mat4 scaleMat{ glm::scale(glm::mat4{ 1.f }, scale) };

        glm::vec3 translation{ (m_TransformedAABBs[index].max + m_TransformedAABBs[index].min) / 2.f };
        glm::mat4 translationMat{ glm::translate(glm::mat4{1.f}, translation) };

        m_ModelMatrices[index] = translationMat * scaleMat;
    }
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
        tMinAABB = AABBCalculator::MinVec(tAABB, tMinAABB);
        tMaxAABB = AABBCalculator::MaxVec(tAABB, tMaxAABB);
        // (xMax, yMin, zMax)
        tAABB = modelMatrix * glm::vec4{ max.x, min.y, max.z, 1.f };
        tMinAABB = AABBCalculator::MinVec(tAABB, tMinAABB);
        tMaxAABB = AABBCalculator::MaxVec(tAABB, tMaxAABB);
        // (xMin, yMin, zMax)
        tAABB = modelMatrix * glm::vec4{ min.x, min.y, max.z, 1.f };
        tMinAABB = AABBCalculator::MinVec(tAABB, tMinAABB);
        tMaxAABB = AABBCalculator::MaxVec(tAABB, tMaxAABB);
        // (xMin, yMax, zMin)
        tAABB = modelMatrix * glm::vec4{ min.x, max.y, min.z, 1.f };
        tMinAABB = AABBCalculator::MinVec(tAABB, tMinAABB);
        tMaxAABB = AABBCalculator::MaxVec(tAABB, tMaxAABB);
        // (xMax, yMax, zMin)
        tAABB = modelMatrix * glm::vec4{ max.x, max.y, min.z, 1.f };
        tMinAABB = AABBCalculator::MinVec(tAABB, tMinAABB);
        tMaxAABB = AABBCalculator::MaxVec(tAABB, tMaxAABB);
        // (xMax, yMax, zMax)
        tAABB = modelMatrix * glm::vec4{ max.x, max.y, max.z, 1.f };
        tMinAABB = AABBCalculator::MinVec(tAABB, tMinAABB);
        tMaxAABB = AABBCalculator::MaxVec(tAABB, tMaxAABB);
        // (xMin, yMax, zMax)
        tAABB = modelMatrix * glm::vec4{ min.x, max.y, max.z, 1.f };
        tMinAABB = AABBCalculator::MinVec(tAABB, tMinAABB);
        tMaxAABB = AABBCalculator::MaxVec(tAABB, tMaxAABB);

        m_TransformedAABBs[i].min = tMinAABB;
        m_TransformedAABBs[i].max = tMaxAABB;
    }
}


std::vector<Vertex> CollisionComponent::m_Vertices{ glm::vec3{ -0.5f, -0.5f, -0.5f },
                                                    glm::vec3{ -0.5f, -0.5f,  0.5f },
                                                    glm::vec3{ -0.5f,  0.5f, -0.5f },
                                                    glm::vec3{ -0.5f,  0.5f,  0.5f },
                                                    glm::vec3{  0.5f, -0.5f, -0.5f },
                                                    glm::vec3{  0.5f, -0.5f,  0.5f },
                                                    glm::vec3{  0.5f,  0.5f, -0.5f },
                                                    glm::vec3{  0.5f,  0.5f,  0.5f } };

std::vector<uint32_t> CollisionComponent::m_Indices{ 0, 1, 2,  1, 2, 3,  0, 4, 6,  6, 0, 2,  1, 5, 7,  7, 1, 3,  4, 5, 6,  6, 7, 2 };
