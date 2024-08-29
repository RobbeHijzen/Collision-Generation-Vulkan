#pragma once

#include "Vulkanbase/Helperstructs/BaseComponent.h"
#include "VulkanBase/HelperStructs/IRenderable.h"
#include "PhysicsGame/Meshes/Mesh.h"
#include "VulkanBase/HelperStructs/HelperStructs.h"


class CollisionComponent : public BaseComponent, public IRenderable
{
public:

	CollisionComponent(Mesh* pParent, bool isStaticMesh, int aabbDepth);
	virtual void GameStart() override;

	bool HasStaticCollision() const { return m_HasStaticCollision; }

	auto GetAABBs() const { return m_TransformedAABBs; }

	bool IsInstanceable() const override { return true; }
	std::vector<glm::mat4> GetModelMatrices() const override { return m_ModelMatrices; }

	const std::vector<uint32_t>& GetIndices() const override { return m_Indices; };
	const std::vector<Vertex>& GetVertices() const override { return m_Vertices; }

	uint32_t GetRenderID() const override { return m_RenderID.value(); }
	void SetRenderID(uint32_t index) override { m_RenderID = index; }

	PipelinesEnum GetPipelineID() const override { return m_PipelineID; }
	std::string GetDiffuseString() const { return "Resources/texs/Manny_BaseColor.png"; };

	void Render(VkCommandBuffer buffer) const override;

	bool IsHidden() const override { return m_IsHidden; }
	void SetHidden(bool newHidden) { m_IsHidden = newHidden; }

private:

	bool m_IsHidden{ false };

	std::vector<glm::mat4> m_ModelMatrices{};
	void UpdateModelMatrix();

	bool m_HasStaticCollision;

	std::vector<AABB> m_AABBs{};
	std::vector<AABB> m_TransformedAABBs{};

	void CalculateTransformedAABBs();

	static std::vector<Vertex> m_Vertices;
	static std::vector<uint32_t> m_Indices;

	std::optional<uint32_t> m_RenderID{};
	PipelinesEnum m_PipelineID{ PipelinesEnum::linesOnly };

};
