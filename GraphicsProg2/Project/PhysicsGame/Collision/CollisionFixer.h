#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "PhysicsGame/Meshes/Mesh.h"
#include "VulkanBase/Scene/Object.h"
#include "VulkanBase/HelperStructs/HelperStructs.h"

using CollisionInfo = std::pair<bool, std::pair<AABB, AABB>>;

class CollisionComponent;

class CollisionFixer
{
public:

	static void FixCollisions(std::vector<Object*> meshes);

	static bool IsOnGround(CollisionComponent* collisionComp, std::vector<Object*> sceneMeshes);

	static CollisionInfo AreColliding(std::vector<AABB> aabbs1, std::vector<AABB> aabbs2, int& i, int& j);
	static CollisionInfo AreColliding(CollisionComponent* col1, CollisionComponent* col2);
	static CollisionInfo AreColliding(CollisionComponent* col1, CollisionComponent* col2, int& i, int& j);
	static CollisionInfo AreColliding(CollisionComponent* col, std::vector<AABB> aabbs2 );

private:

	static void HandleCollision(AABB aabb1, AABB aabb2, CollisionComponent* col1, CollisionComponent* col2);

	static bool AreBothStaticMeshes(CollisionComponent* col1, CollisionComponent* col2);
	static bool AreBothNonStaticMeshes(CollisionComponent* col1, CollisionComponent* col2);

	static bool AreIntervalsOverlapping(float a1, float a2, float A1, float A2);

	static std::pair<glm::vec3, glm::vec3> CalculateCollisionDistances(AABB aabb1, AABB aabb2);
};