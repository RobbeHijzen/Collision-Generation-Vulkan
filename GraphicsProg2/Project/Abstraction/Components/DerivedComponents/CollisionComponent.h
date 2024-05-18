#pragma once

#include "Abstraction/Components/BaseComponent.h"
#include "Abstraction/Meshes/Mesh.h"


class CollisionComponent : public BaseComponent
{
public:

	CollisionComponent(Mesh* pParent, bool isStaticMesh);
	virtual void GameStart() override;

	bool HasStaticCollision() const { return m_HasStaticCollision; }

	glm::vec3 GetMinAABB() const { return m_TransformedMinAABB; }
	glm::vec3 GetMaxAABB() const { return m_TransformedMaxAABB; }

private:
	bool m_HasStaticCollision;

	glm::vec3 m_MinAABB{};
	glm::vec3 m_MaxAABB{};

	glm::vec3 m_TransformedMinAABB{};
	glm::vec3 m_TransformedMaxAABB{};

	void CalculateAABB();
	void CalculateTransformedAABB();

	glm::vec3 MinVec(const glm::vec3& v1, const glm::vec3& v2);
	glm::vec3 MaxVec(const glm::vec3& v1, const glm::vec3& v2);


};
