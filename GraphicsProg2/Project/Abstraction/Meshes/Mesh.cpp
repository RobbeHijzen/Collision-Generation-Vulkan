#include "Mesh.h"
#include "Abstraction/Utils.h"
#include "Abstraction/Time/Time.h"

#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(int loadIndex, bool calculateNormals, std::string objPath, std::string diffuseString, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    : m_LoadIndex{loadIndex}
    , m_DiffuseString{diffuseString}
{
    LoadOBJ(objPath, calculateNormals);


    m_WorldPos = translation;
    m_BaseRot = glm::radians(rotation);
    m_WorldScale = scale;

    m_TranslationMatrix = glm::translate(glm::mat4{ 1.f }, {m_WorldPos.x, m_WorldPos.y, -m_WorldPos.z});
    m_RotationMatrix = glm::rotate(glm::mat4{ 1.f }, m_WorldRot.z + m_BaseRot.z, glm::vec3{ 0.f, 0.f, 1.f })
                     * glm::rotate(glm::mat4{ 1.f }, -m_WorldRot.y - m_BaseRot.y, glm::vec3{ 0.f, 1.f, 0.f })
                     * glm::rotate(glm::mat4{ 1.f }, m_WorldRot.x + m_BaseRot.x, glm::vec3{ 1.f, 0.f, 0.f });
    m_ScaleMatrix = glm::scale(glm::mat4{ 1.f }, m_WorldScale);


    AddObserver(new Observer(GameEvents::ModelMatrixChanged, [&] { this->CalculateWorldMatrix(); }));
    
    NotifyObservers(GameEvents::ModelMatrixChanged);
}

void Mesh::LoadOBJ(std::string objPath, bool calculateNormals)
{
    if (std::ifstream input{ "Resources/VertexInfo.txt", std::ios::binary }; input.is_open())
    {
        while (!input.eof())
        {
            int loadIndex{ -1 };
            int vertexLoadCount{};
            int indexLoadCount{};

            input.read((char*)&loadIndex, sizeof(int));
            input.read((char*)&vertexLoadCount, sizeof(int));
            input.read((char*)&indexLoadCount, sizeof(int));

            if (loadIndex == m_LoadIndex)
            {
                m_Vertices.resize(vertexLoadCount);
                input.read((char*)&m_Vertices[0], sizeof(Vertex) * vertexLoadCount);

                m_Indices.resize(indexLoadCount);
                input.read((char*)&m_Indices[0], sizeof(uint32_t) * indexLoadCount);

                return;
            }
            else
            {
                // Vertex skipping
                const int sizeV{ static_cast<int>(sizeof(Vertex)) * vertexLoadCount };
                char* arrV{ new char[sizeV] };
                input.read(arrV, sizeV);
                delete[] arrV;

                // index skipping
                const int sizeI{ static_cast<int>(sizeof(uint32_t)) * indexLoadCount };
                char* arrI{ new char[sizeI] };
                input.read(arrI, sizeI);
                delete[] arrI;
            }
        }
    }

    if (std::ofstream output{ "Resources/VertexInfo.txt", std::ios::app | std::ios::binary }; output.is_open())
    {
        bool succeeded = ParseOBJ(objPath, m_Vertices, m_Indices, calculateNormals);
        assert(succeeded);

        int vertexLoadCount{ static_cast<int>(m_Vertices.size()) };
        int indexLoadCount{ static_cast<int>(m_Indices.size()) };

        output.write((const char*)&m_LoadIndex, sizeof(int));
        output.write((const char*)&vertexLoadCount, sizeof(int));
        output.write((const char*)&indexLoadCount, sizeof(int));

        output.write((const char*)&m_Vertices[0], sizeof(Vertex) * vertexLoadCount);
        output.write((const char*)&m_Indices[0], sizeof(uint32_t) * indexLoadCount);
    }
}

void Mesh::Render(VkCommandBuffer buffer) const
{
    vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh::GameStart()
{
    for (auto& component : m_Components)
    {
        component->GameStart();
    }
}

void Mesh::Update(GLFWwindow* window)
{
    for (auto& component : m_Components)
    {
        component->Update(window);
    }

    Translate(m_Velocity * Time::GetInstance()->GetDeltaTime());
}

void Mesh::LateUpdate()
{
    for (auto& component : m_Components)
    {
        component->LateUpdate();
    }
}

void Mesh::Translate(glm::vec3 addedPos)
{
    m_WorldPos += addedPos;
    m_TranslationMatrix = glm::translate(glm::mat4{ 1.f }, { m_WorldPos.x, m_WorldPos.y, -m_WorldPos.z });

    NotifyObservers(GameEvents::ModelMatrixChanged);
}

void Mesh::Rotate(glm::vec3 addedRot)
{
    m_WorldRot += addedRot;
    m_RotationMatrix = glm::rotate(glm::mat4{ 1.f }, m_WorldRot.z + m_BaseRot.z, glm::vec3{ 0.f, 0.f, 1.f })
                     * glm::rotate(glm::mat4{ 1.f }, -m_WorldRot.y - m_BaseRot.y, glm::vec3{ 0.f, 1.f, 0.f })
                     * glm::rotate(glm::mat4{ 1.f }, m_WorldRot.x + m_BaseRot.x, glm::vec3{ 1.f, 0.f, 0.f });

    NotifyObservers(GameEvents::ModelMatrixChanged);
}

void Mesh::Scale(glm::vec3 addedScale)
{
    m_WorldScale += addedScale;
    m_ScaleMatrix = glm::scale(glm::mat4{ 1.f }, m_WorldScale);

    NotifyObservers(GameEvents::ModelMatrixChanged);
}

void Mesh::CalculateWorldMatrix()
{
    m_ModelMatrix = m_TranslationMatrix * m_RotationMatrix * m_ScaleMatrix;
}

