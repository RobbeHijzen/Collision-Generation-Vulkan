#include "Mesh.h"
#include "Utils.h"
#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(std::string objPath, std::string diffuseString, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    : m_DiffuseString{diffuseString}
{
    bool succeeded = ParseOBJ(objPath, m_Vertices, m_Indices);
    assert(succeeded);


    m_ModelMatrix = glm::translate(glm::mat4{ 1.f }, translation)
                    * (glm::rotate(glm::mat4{ 1.f }, glm::radians(rotation.z), glm::vec3{ 0.f, 0.f, 1.f })
                    * glm::rotate(glm::mat4{ 1.f }, glm::radians(rotation.y), glm::vec3{ 0.f, 1.f, 0.f })
                    * glm::rotate(glm::mat4{ 1.f }, glm::radians(rotation.x), glm::vec3{ 1.f, 0.f, 0.f }))
                    * glm::scale(glm::mat4{ 1.f }, scale);
}

void Mesh::Draw(VkCommandBuffer buffer) const
{
    vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}
