#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "Abstraction/Meshes/Mesh.h"

class CollisionComponent;

class CollisionFixer
{
public:

	static void FixCollisions(std::vector<Mesh*> meshes);
	static bool AreColliding(CollisionComponent* mesh1, glm::vec3 min2, glm::vec3 max2);

private:

	static void HandleCollision(CollisionComponent* mesh1, CollisionComponent* mesh2);
	static bool AreColliding(CollisionComponent* mesh1, CollisionComponent* mesh2);

	static bool AreBothStaticMeshes(CollisionComponent* mesh1, CollisionComponent* mesh2);
	static bool AreBothNonStaticMeshes(CollisionComponent* mesh1, CollisionComponent* mesh2);

	static bool AreIntervalsOverlapping(float a1, float a2, float A1, float A2);

	static std::pair<glm::vec3, glm::vec3> CalculateCollisionDistances(CollisionComponent* mesh1, CollisionComponent* mesh2);
};