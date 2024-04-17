#include "Mesh.h"
#include "Utils.h"

#include "Shaders/ShaderManager.h"

void Mesh::SetMeshIndex(uint32_t index)
{
    assert(!m_MeshIndex.has_value());
    m_MeshIndex = index;

    ShaderManager::GetInstance().GetShaders()[m_ShaderIndex]->AddReferencedMeshIndex(m_MeshIndex.value());
}

Mesh3D::Mesh3D(std::string objPath, std::string diffuseString, uint32_t shaderIndex, glm::mat4 modelMatrix)
    : Mesh{ shaderIndex, modelMatrix }
    , m_DiffuseString{ diffuseString }
{
    bool succeeded = Utils::ParseOBJ(objPath, m_3DVertices, m_Indices);
    assert(succeeded);

    for (const auto& vertex3D : m_3DVertices)
    {
        m_Vertices.emplace_back(vertex3D);
    }
}
void Mesh3D::Draw(VkCommandBuffer buffer) const
{
    vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}


Mesh2D::Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix, RectangleInfo rec, glm::vec3 color)
    : Mesh2D(shaderIndex, modelMatrix)
{
    UseRectangleInfo(rec);
    m_Vertices.clear();
    for (auto& vertex : m_2DVertices)
    {
        vertex.color = color;
        m_Vertices.emplace_back(vertex);
    }
}
Mesh2D::Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix, OvalInfo oval, glm::vec3 color)
    : Mesh2D(shaderIndex, modelMatrix)
{
    UseOvalInfo(oval);
    m_Vertices.clear();
    for (auto& vertex : m_2DVertices)
    {
        vertex.color = color;
        m_Vertices.emplace_back(vertex);
    }
}
Mesh2D::Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix)
    : Mesh{ shaderIndex, modelMatrix }
{
    m_Indices = { 0, 1, 2 };
}
void Mesh2D::Draw(VkCommandBuffer buffer) const
{
    vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh2D::UseRectangleInfo(RectangleInfo rec)
{
    m_2DVertices.clear();
    m_Indices.clear();

    // Add vertices of the rectangle
    m_2DVertices.emplace_back(Vertex2D{ glm::vec2{rec.left, rec.top }});
    m_2DVertices.emplace_back(Vertex2D{ glm::vec2{rec.right, rec.top}});
    m_2DVertices.emplace_back(Vertex2D{ glm::vec2{rec.right, rec.bot}});
    m_2DVertices.emplace_back(Vertex2D{ glm::vec2{rec.left, rec.bot }});

    // Add indices to form two triangles (rectangular shape)
    m_Indices.emplace_back(0);
    m_Indices.emplace_back(1);
    m_Indices.emplace_back(2);

    m_Indices.emplace_back(2);
    m_Indices.emplace_back(3);
    m_Indices.emplace_back(0);
}
void Mesh2D::UseOvalInfo(OvalInfo oval)
{
    m_2DVertices.clear();
    m_Indices.clear();

    float angleIncrement = 2.0f * 3.14159265f / oval.segments;

    for (int i = 0; i < oval.segments; ++i)
    {
        float angle = i * angleIncrement;
        float x = oval.radiusX * cos(angle);
        float y = oval.radiusY * sin(angle);
        m_2DVertices.emplace_back(Vertex2D{ {x, y}});
    }

    // Add indices to form triangles connecting the vertices
    for (int i = 1; i < oval.segments - 1; ++i)
    {
        m_Indices.emplace_back(0);
        m_Indices.emplace_back(i);
        m_Indices.emplace_back(i + 1);
    }
}

