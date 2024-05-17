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
}

void Mesh::Rotate(glm::vec3 addedRot)
{
    m_WorldRot += addedRot;
    m_RotationMatrix = glm::rotate(glm::mat4{ 1.f }, m_WorldRot.z + m_BaseRot.z, glm::vec3{ 0.f, 0.f, 1.f })
                     * glm::rotate(glm::mat4{ 1.f }, -m_WorldRot.y - m_BaseRot.y, glm::vec3{ 0.f, 1.f, 0.f })
                     * glm::rotate(glm::mat4{ 1.f }, m_WorldRot.x + m_BaseRot.x, glm::vec3{ 1.f, 0.f, 0.f });

    CalculateWorldMatrix();
}

void Mesh::Scale(glm::vec3 addedScale)
{
    m_WorldScale += addedScale;
    m_ScaleMatrix = glm::scale(glm::mat4{ 1.f }, m_WorldScale);

    CalculateWorldMatrix();
}

void Mesh::CalculateWorldMatrix()
{
    m_ModelMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
}
