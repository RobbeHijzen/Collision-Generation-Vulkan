#include "CollisionFixer.h"

void CollisionFixer::FixCollisions(std::vector<Mesh*> meshes)
{
	for (int i{}; i < meshes.size(); ++i)
	{
		for (int j{ i + 1 }; j < meshes.size(); ++j)
		{
			if (AreColliding(meshes[i], meshes[j]))
			{
				std::cout << "Meshes are overlapping\n";
				HandleCollision(meshes[i], meshes[j]);
			}
		}
	}
}

void CollisionFixer::HandleCollision(Mesh* mesh1, Mesh* mesh2)
{
	if (AreBothStaticMeshes(mesh1, mesh2)) return;

	if (!AreBothNonStaticMeshes(mesh1, mesh2))
	{
		// Just move the nonstatic mesh
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

	float x1{min1.x};
	float x2{min2.x};
	float y1{min1.y};
	float y2{min2.y};
	float z1{min1.z};
	float z2{min2.z};

	glm::vec3 max1{mesh1->GetMaxAABB()};
	glm::vec3 max2{mesh2->GetMaxAABB()};

	float X1{max1.x};
	float X2{max2.x};
	float Y1{max1.y};
	float Y2{max2.y};
	float Z1{max1.z};
	float Z2{max2.z};



	return AreIntervalsOverlapping(x1, x2, X1, X2) &&
		   AreIntervalsOverlapping(y1, y2, Y1, Y2) &&
		   AreIntervalsOverlapping(z1, z2, Z1, Z2);
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
