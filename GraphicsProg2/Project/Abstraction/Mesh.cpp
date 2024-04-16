#include "Mesh.h"
#include "Utils.h"

#include "Shaders/ShaderManager.h"

Mesh3D::Mesh3D(std::string objPath, std::string diffuseString, uint32_t shaderIndex, glm::mat4 modelMatrix)
	: Mesh{shaderIndex, modelMatrix}
	, m_DiffuseString{diffuseString}
{
    std::vector<Vertex3D> vertices3D{};

	bool succeeded = Utils::ParseOBJ(objPath, vertices3D, m_Indices);
	assert(succeeded);

    for (const auto& vertex3D : vertices3D)
    {
        m_Vertices.emplace_back(vertex3D);
    }
}
void Mesh3D::Draw(VkCommandBuffer buffer) const
{
	vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}



Mesh2D::Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix, RectangleInfo rec)
	: Mesh2D(shaderIndex, modelMatrix)
{
	UseRectangleInfo(rec);
}
Mesh2D::Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix, OvalInfo oval)
	: Mesh2D(shaderIndex, modelMatrix)
{
	UseOvalInfo(oval);
}
Mesh2D::Mesh2D(uint32_t shaderIndex, glm::mat4 modelMatrix)
	: Mesh{ shaderIndex, modelMatrix }
{
}
void Mesh2D::Draw(VkCommandBuffer buffer) const
{
	vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh2D::UseRectangleInfo(RectangleInfo rec)
{
    m_Vertices.clear();
    m_Indices.clear();

    // Add vertices of the rectangle
    m_Vertices.emplace_back(Vertex2D{ glm::vec2{rec.left, rec.top }, glm::vec3{1.f, 1.f, 1.f} });
    m_Vertices.emplace_back(Vertex2D{ glm::vec2{rec.right, rec.top}, glm::vec3{1.f, 1.f, 1.f} }); 
    m_Vertices.emplace_back(Vertex2D{ glm::vec2{rec.right, rec.bot}, glm::vec3{1.f, 1.f, 1.f} }); 
    m_Vertices.emplace_back(Vertex2D{ glm::vec2{rec.left, rec.bot }, glm::vec3{1.f, 1.f, 1.f} }); 

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
    m_Vertices.clear();
    m_Indices.clear();

    float angleIncrement = 2.0f * 3.14159265f / oval.segments;

    for (int i = 0; i < oval.segments; ++i) 
    {
        float angle = i * angleIncrement;
        float x = oval.centerX + oval.radius * cos(angle);
        float y = oval.centerY + oval.radius * sin(angle);
        m_Vertices.emplace_back(Vertex2D{ {x, y}, {1.f, 1.f, 1.f} });
    }

    // Add indices to form triangles connecting the vertices
    for (int i = 1; i < oval.segments - 1; ++i) 
    {
        m_Indices.emplace_back(0);         
        m_Indices.emplace_back(i);         
        m_Indices.emplace_back(i + 1);     
    }

    // Connect last vertex with the first to close the oval
    m_Indices.emplace_back(0);
    m_Indices.emplace_back(oval.segments - 1);
    m_Indices.emplace_back(1);
}

