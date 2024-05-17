#include "Mesh.h"
#include "Abstraction/Utils.h"
#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(std::string objPath, std::string diffuseString, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    : m_DiffuseString{diffuseString}
{
    bool succeeded = ParseOBJ(objPath, m_Vertices, m_Indices);
    assert(succeeded);


    m_WorldPos = translation;
    m_BaseRot = glm::radians(rotation);
    m_WorldScale = scale;

    m_TranslationMatrix = glm::translate(glm::mat4{ 1.f }, {m_WorldPos.x, m_WorldPos.y, -m_WorldPos.z});
    m_RotationMatrix = glm::rotate(glm::mat4{ 1.f }, m_WorldRot.z + m_BaseRot.z, glm::vec3{ 0.f, 0.f, 1.f })
                     * glm::rotate(glm::mat4{ 1.f }, -m_WorldRot.y - m_BaseRot.y, glm::vec3{ 0.f, 1.f, 0.f })
                     * glm::rotate(glm::mat4{ 1.f }, m_WorldRot.x + m_BaseRot.x, glm::vec3{ 1.f, 0.f, 0.f });
    m_ScaleMatrix = glm::scale(glm::mat4{ 1.f }, m_WorldScale);

    CalculateWorldMatrix();
    CalculateAABB();
    CalculateTransformedAABB();
}

void Mesh::Draw(VkCommandBuffer buffer) const
{
    vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}


void Mesh::Translate(glm::vec3 addedPos)
{
    m_WorldPos += addedPos;
    m_TranslationMatrix = glm::translate(glm::mat4{ 1.f }, { m_WorldPos.x, m_WorldPos.y, -m_WorldPos.z });

    CalculateWorldMatrix();
    CalculateTransformedAABB();
}

void Mesh::Rotate(glm::vec3 addedRot)
{
    m_WorldRot += addedRot;
    m_RotationMatrix = glm::rotate(glm::mat4{ 1.f }, m_WorldRot.z + m_BaseRot.z, glm::vec3{ 0.f, 0.f, 1.f })
                     * glm::rotate(glm::mat4{ 1.f }, -m_WorldRot.y - m_BaseRot.y, glm::vec3{ 0.f, 1.f, 0.f })
                     * glm::rotate(glm::mat4{ 1.f }, m_WorldRot.x + m_BaseRot.x, glm::vec3{ 1.f, 0.f, 0.f });

    CalculateWorldMatrix();
    CalculateTransformedAABB();
}

void Mesh::Scale(glm::vec3 addedScale)
{
    m_WorldScale += addedScale;
    m_ScaleMatrix = glm::scale(glm::mat4{ 1.f }, m_WorldScale);

    CalculateWorldMatrix();
    CalculateTransformedAABB();
}

void Mesh::CalculateWorldMatrix()
{
    m_ModelMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
}

void Mesh::CalculateAABB()
{
    if (m_Vertices.size() > 0)
    {
        m_MinAABB = m_Vertices[0].pos;
        m_MaxAABB = m_Vertices[0].pos;
        for (const auto& vertex : m_Vertices)
        {
            m_MinAABB = MinVec(vertex.pos, m_MinAABB);
            m_MaxAABB = MaxVec(vertex.pos, m_MaxAABB);
        }
    }
}

void Mesh::CalculateTransformedAABB()
{
    // This transforms the 8 vertices according to the transform of the triangleMesh

    // Begin
    glm::vec3 tMinAABB = m_ModelMatrix * glm::vec4{ m_MinAABB, 1.f};
    glm::vec3 tMaxAABB = tMinAABB;

    // (xMax, yMin, zMin)
    glm::vec3 tAABB = m_ModelMatrix * glm::vec4{ m_MaxAABB.x, m_MinAABB.y, m_MinAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMax, yMin, zMax)
    tAABB = m_ModelMatrix * glm::vec4{ m_MaxAABB.x, m_MinAABB.y, m_MaxAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMin, yMin, zMax)
    tAABB = m_ModelMatrix * glm::vec4{ m_MinAABB.x, m_MinAABB.y, m_MaxAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMin, yMax, zMin)
    tAABB = m_ModelMatrix * glm::vec4{ m_MinAABB.x, m_MaxAABB.y, m_MinAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMax, yMax, zMin)
    tAABB = m_ModelMatrix * glm::vec4{ m_MaxAABB.x, m_MaxAABB.y, m_MinAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMax, yMax, zMax)
    tAABB = m_ModelMatrix * glm::vec4{ m_MaxAABB.x, m_MaxAABB.y, m_MaxAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);
    // (xMin, yMax, zMax)
    tAABB = m_ModelMatrix * glm::vec4{ m_MinAABB.x, m_MaxAABB.y, m_MaxAABB.z, 1.f };
    tMinAABB = MinVec(tAABB, tMinAABB);
    tMaxAABB = MaxVec(tAABB, tMaxAABB);

    m_TransformedMinAABB = tMinAABB;
    m_TransformedMaxAABB = tMaxAABB;

    //std::cout << "MinAABB.z: " << m_TransformedMinAABB.z << "   MaxAABB.z: " << m_TransformedMaxAABB.z << "\n";
}

glm::vec3 Mesh::MinVec(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::vec3{
        min(v1.x, v2.x),
        min(v1.y, v2.y),
        min(v1.z, v2.z)
    };
}

glm::vec3 Mesh::MaxVec(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::vec3{
       max(v1.x, v2.x),
       max(v1.y, v2.y),
       max(v1.z, v2.z)
    };
}
