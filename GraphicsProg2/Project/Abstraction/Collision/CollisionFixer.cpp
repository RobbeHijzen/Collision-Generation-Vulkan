#include "CollisionFixer.h"

void CollisionFixer::FixCollisions(std::vector<Mesh*> meshes)
{
	for (int i{}; i < meshes.size(); ++i)
	{
		for (int j{ i + 1 }; j < meshes.size(); ++j)
		{
			Mesh* mesh1{ meshes[i] };
			Mesh* mesh2{ meshes[j] };
			if (AreColliding(mesh1, mesh2))
			{
				if (AreBothStaticMeshes(mesh1, mesh2)) continue;

				std::cout << "Meshes are overlapping\n";
				HandleCollision(mesh1, mesh2);
			}
		}
	}
}

void CollisionFixer::HandleCollision(Mesh* mesh1, Mesh* mesh2)
{
	if (!AreBothNonStaticMeshes(mesh1, mesh2))
	{
		// Just move the nonstatic mesh

		// Swaps mesh1 to be the nonstatic mesh
		if (mesh1->IsStaticMesh())
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
			mesh1->Translate({ distances.first.x * distances.second.x, 0.f, 0.f });
		}
		else if (minDistance == distances.first.y)
		{
			mesh1->Translate({ 0.f, distances.first.y * distances.second.y, 0.f });
		}
		else if (minDistance == distances.first.z)
		{
			mesh1->Translate({ 0.f, 0.f, distances.first.z * distances.second.z });
		}
	}
	else
	{
		// Move both meshes halfway
	}
}

bool CollisionFixer::AreColliding(Mesh* mesh1, Mesh* mesh2)
{
	glm::vec3 min1{mesh1->GetMinAABB()};
	glm::vec3 min2{mesh2->GetMinAABB()};
	glm::vec3 max1{mesh1->GetMaxAABB()};
	glm::vec3 max2{mesh2->GetMaxAABB()};


	return AreIntervalsOverlapping(min1.x, min2.x, max1.x, max2.x) &&
		   AreIntervalsOverlapping(min1.y, min2.y, max1.y, max2.y) &&
		   AreIntervalsOverlapping(min1.z, min2.z, max1.z, max2.z);
}

bool CollisionFixer::AreBothStaticMeshes(Mesh* mesh1, Mesh* mesh2)
{
	return mesh1->IsStaticMesh() && mesh2->IsStaticMesh();
}

bool CollisionFixer::AreBothNonStaticMeshes(Mesh* mesh1, Mesh* mesh2)
{
	return !mesh1->IsStaticMesh() && !mesh2->IsStaticMesh();
}

bool CollisionFixer::AreIntervalsOverlapping(float a1, float a2, float A1, float A2)
{
	return
		a2 < A1 &&
		a1 < A2;
}

std::pair<glm::vec3, glm::vec3> CollisionFixer::CalculateCollisionDistances(Mesh* mesh1, Mesh* mesh2)
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
