#include "CollisionFixer.h"
#include "Abstraction/Meshes/Mesh.h"
#include "Abstraction/Components/DerivedComponents/CollisionComponent.h"

void CollisionFixer::FixCollisions(std::vector<Mesh*> meshes)
{
	for (int i{}; i < meshes.size(); ++i)
	{
		for (int j{ i + 1 }; j < meshes.size(); ++j)
		{
			Mesh* mesh1{ meshes[i] };
			Mesh* mesh2{ meshes[j] };

			CollisionComponent* col1{ mesh1->GetComponent<CollisionComponent>().get()};
			CollisionComponent* col2{ mesh2->GetComponent<CollisionComponent>().get() };

			if (col1 && col2)
			{
				if (AreColliding(col1, col2))
				{
					if (AreBothStaticMeshes(col1, col2)) continue;
					HandleCollision(col1, col2);
				}
			}
		}
	}
}

void CollisionFixer::HandleCollision(CollisionComponent* mesh1, CollisionComponent* mesh2)
{
	if (!AreBothNonStaticMeshes(mesh1, mesh2))
	{
		// Just move the nonstatic mesh

		// Swaps mesh1 to be the nonstatic mesh
		if (mesh1->HasStaticCollision())
		{
			auto temp{ mesh1 };
			mesh1 = mesh2;
			mesh2 = temp;
		}

		// Changes mesh1's position to not collide anymore

		std::pair<glm::vec3, glm::vec3> distances{ CalculateCollisionDistances(mesh1, mesh2) };
		float minDistance{min(min(distances.first.x, distances.first.y), distances.first.z)};
		if (minDistance == distances.first.x)
		{
			mesh1->GetOwner()->Translate({ distances.first.x * distances.second.x, 0.f, 0.f });
		}
		else if (minDistance == distances.first.y)
		{
			mesh1->GetOwner()->Translate({ 0.f, distances.first.y * distances.second.y, 0.f });
		}
		else if (minDistance == distances.first.z)
		{
			mesh1->GetOwner()->Translate({ 0.f, 0.f, distances.first.z * distances.second.z });
		}
	}
	else
	{
		// Move both meshes halfway
		std::pair<glm::vec3, glm::vec3> distances{ CalculateCollisionDistances(mesh1, mesh2) };
		float minDistance{ min(min(distances.first.x, distances.first.y), distances.first.z) };
		if (minDistance == distances.first.x)
		{
			mesh1->GetOwner()->Translate({ distances.first.x * distances.second.x / 2.f, 0.f, 0.f });
			mesh2->GetOwner()->Translate({ distances.first.x * distances.second.x / -2.f, 0.f, 0.f });
		}
		else if (minDistance == distances.first.y)
		{
			mesh1->GetOwner()->Translate({ 0.f, distances.first.y * distances.second.y / 2.f, 0.f });
			mesh2->GetOwner()->Translate({ 0.f, distances.first.y * distances.second.y / -2.f, 0.f });
		}
		else if (minDistance == distances.first.z)
		{
			mesh1->GetOwner()->Translate({ 0.f, 0.f, distances.first.z * distances.second.z / 2.f });
			mesh2->GetOwner()->Translate({ 0.f, 0.f, distances.first.z * distances.second.z / -2.f });
		}
	}
}

bool CollisionFixer::AreColliding(CollisionComponent* mesh1, CollisionComponent* mesh2)
{
	glm::vec3 min1{mesh1->GetMinAABB()};
	glm::vec3 min2{mesh2->GetMinAABB()};
	glm::vec3 max1{mesh1->GetMaxAABB()};
	glm::vec3 max2{mesh2->GetMaxAABB()};


	return AreIntervalsOverlapping(min1.x, min2.x, max1.x, max2.x) &&
		   AreIntervalsOverlapping(min1.y, min2.y, max1.y, max2.y) &&
		   AreIntervalsOverlapping(min1.z, min2.z, max1.z, max2.z);
}

bool CollisionFixer::AreColliding(CollisionComponent* mesh1, glm::vec3 min2, glm::vec3 max2)
{
	glm::vec3 min1{ mesh1->GetMinAABB() };
	glm::vec3 max1{ mesh1->GetMaxAABB() };

	return AreIntervalsOverlapping(min1.x, min2.x, max1.x, max2.x) &&
		   AreIntervalsOverlapping(min1.y, min2.y, max1.y, max2.y) &&
		   AreIntervalsOverlapping(min1.z, min2.z, max1.z, max2.z);
}

bool CollisionFixer::AreBothStaticMeshes(CollisionComponent* mesh1, CollisionComponent* mesh2)
{
	return mesh1->HasStaticCollision() && mesh2->HasStaticCollision();
}

bool CollisionFixer::AreBothNonStaticMeshes(CollisionComponent* mesh1, CollisionComponent* mesh2)
{
	return !mesh1->HasStaticCollision() && !mesh2->HasStaticCollision();
}

bool CollisionFixer::AreIntervalsOverlapping(float a1, float a2, float A1, float A2)
{
	return
		a2 < A1 &&
		a1 < A2;
}

std::pair<glm::vec3, glm::vec3> CollisionFixer::CalculateCollisionDistances(CollisionComponent* mesh1, CollisionComponent* mesh2)
{
	std::pair<glm::vec3, glm::vec3> distances{};

	glm::vec3 min1{ mesh1->GetMinAABB() };
	glm::vec3 min2{ mesh2->GetMinAABB() };
	glm::vec3 max1{ mesh1->GetMaxAABB() };
	glm::vec3 max2{ mesh2->GetMaxAABB() };

	// X Calculation
	float val1{ abs(max1.x - min2.x) };
	float val2{ abs(max2.x - min1.x) };
	if (val1 <= val2)
	{
		distances.first.x = val1;
		distances.second.x = -1;
	}
	else
	{
		distances.first.x = val2;
		distances.second.x = 1;
	}

	// Y Calculation
	val1 = abs(max1.y - min2.y);
	val2 = abs(max2.y - min1.y);
	if (val1 <= val2)
	{
		distances.first.y = val1;
		distances.second.y = -1;
	}
	else
	{
		distances.first.y = val2;
		distances.second.y = 1;
	}

	// Z Calculation
	val1 = abs(max1.z - min2.z);
	val2 = abs(max2.z - min1.z);
	if (val1 <= val2)
	{
		distances.first.z = val1;
		distances.second.z = 1;
	}
	else
	{
		distances.first.z = val2;
		distances.second.z = -1;
	}

	return distances;
}
