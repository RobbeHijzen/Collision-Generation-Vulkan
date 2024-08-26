#include "CollisionFixer.h"
#include "PhysicsGame/Meshes/Mesh.h"
#include "PhysicsGame/Components/DerivedComponents/CollisionComponent.h"

void CollisionFixer::FixCollisions(std::vector<Object*> meshes)
{
	for (int i{}; i < meshes.size(); ++i)
	{
		for (int j{ i + 1 }; j < meshes.size(); ++j)
		{
			int maxIterations{ 10 };
			for(int k{}; k < maxIterations; ++k)
			{
				Object* mesh1{ meshes[i] };
				Object* mesh2{ meshes[j] };

				CollisionComponent* collisionComp1{ mesh1->GetComponent<CollisionComponent>().get() };
				CollisionComponent* collisionComp2{ mesh2->GetComponent<CollisionComponent>().get() };

				if (collisionComp1 && collisionComp2)
				{
					if (AreBothStaticMeshes(collisionComp1, collisionComp2)) break;

					int i2{};
					int j2{};

					CollisionInfo collisionInfo{};
					collisionInfo = AreColliding(collisionComp1, collisionComp2);

					while (collisionInfo.first)
					{

						HandleCollision(collisionInfo.second.first,
										collisionInfo.second.second,
										collisionComp1,
										collisionComp2);

						collisionInfo = AreColliding(collisionComp1, collisionComp2, i2, j2);
					}
					if (!AreColliding(collisionComp1, collisionComp2).first) break;
				}
			}
		}
	}
}

bool CollisionFixer::IsOnGround(CollisionComponent* collisionComp, std::vector<Object*> sceneMeshes)
{
	std::vector<AABB> aabbs{ collisionComp->GetAABBs() };

	std::vector<AABB> groundAABBs{};
	std::vector<AABB> legsAABBs{};

	for (int index{}; index < aabbs.size(); ++index)
	{
		AABB aabb{aabbs[index]};
		AABB groundAABB{};

		groundAABB.min.x = aabb.min.x + FLT_EPSILON;
		groundAABB.max.x = aabb.max.x - FLT_EPSILON;

		groundAABB.min.y = aabb.min.y - FLT_EPSILON;
		groundAABB.max.y = aabb.max.y - FLT_EPSILON;

		groundAABB.min.z = aabb.min.z + FLT_EPSILON;
		groundAABB.max.z = aabb.max.z - FLT_EPSILON;

		groundAABBs.emplace_back(groundAABB);
	}
	for (int index{}; index < aabbs.size(); ++index)
	{
		AABB aabb{ aabbs[index] };
		AABB legsAABB{};

		legsAABB.min.x = aabb.min.x + FLT_EPSILON;
		legsAABB.max.x = aabb.max.x - FLT_EPSILON;

		legsAABB.min.y = aabb.min.y + 8000 * FLT_EPSILON;
		legsAABB.max.y = aabb.max.y - FLT_EPSILON;

		legsAABB.min.z = aabb.min.z + FLT_EPSILON;
		legsAABB.max.z = aabb.max.z - FLT_EPSILON;

		legsAABBs.emplace_back(legsAABB);
	}

	for (auto mesh : sceneMeshes)
	{
		if (auto col = mesh->GetComponent<CollisionComponent>())
		{
			bool areFeetOnGround{ CollisionFixer::AreColliding(col.get(), groundAABBs).first };
			bool isBodyTouchingSomething{ CollisionFixer::AreColliding(col.get(), legsAABBs).first };
			if (areFeetOnGround && !isBodyTouchingSomething)
			{
				return true;
			}
		}
	}

	return false;
}

void CollisionFixer::HandleCollision(AABB aabb1, AABB aabb2, CollisionComponent* col1, CollisionComponent* col2)
{
	if (!AreBothNonStaticMeshes(col1, col2))
	{
		// Just move the nonstatic mesh

		// Swaps mesh1 to be the nonstatic mesh
		if (col1->HasStaticCollision())
		{
			auto temp1{ col1 };
			col1 = col2;
			col2 = temp1;

			auto temp2{ aabb1 };
			aabb1 = aabb2;
			aabb2 = temp2;
		}

		// Changes mesh1's position to not collide anymore

		std::pair<glm::vec3, glm::vec3> distances{ CalculateCollisionDistances(aabb1, aabb2) };
		float minDistance{std::min(std::min(distances.first.x, distances.first.y), distances.first.z)};
		if (minDistance == distances.first.x)
		{
			col1->GetOwner()->Translate({ distances.first.x * distances.second.x, 0.f, 0.f });
		}
		else if (minDistance == distances.first.y)
		{
			col1->GetOwner()->Translate({ 0.f, distances.first.y * distances.second.y, 0.f });
		}
		else if (minDistance == distances.first.z)
		{
			col1->GetOwner()->Translate({ 0.f, 0.f, distances.first.z * distances.second.z });
		}
	}
	else
	{
		// Move both meshes halfway
		std::pair<glm::vec3, glm::vec3> distances{ CalculateCollisionDistances(aabb1, aabb2) };
		float minDistance{ std::min(std::min(distances.first.x, distances.first.y), distances.first.z) };
		if (minDistance == distances.first.x)
		{
			col1->GetOwner()->Translate({ distances.first.x * distances.second.x / 2.f, 0.f, 0.f });
			col2->GetOwner()->Translate({ distances.first.x * distances.second.x / -2.f, 0.f, 0.f });
		}
		else if (minDistance == distances.first.y)
		{
			col1->GetOwner()->Translate({ 0.f, distances.first.y * distances.second.y / 2.f, 0.f });
			col2->GetOwner()->Translate({ 0.f, distances.first.y * distances.second.y / -2.f, 0.f });
		}
		else if (minDistance == distances.first.z)
		{
			col1->GetOwner()->Translate({ 0.f, 0.f, distances.first.z * distances.second.z / 2.f });
			col2->GetOwner()->Translate({ 0.f, 0.f, distances.first.z * distances.second.z / -2.f });
		}
	}
}

CollisionInfo CollisionFixer::AreColliding(std::vector<AABB> aabbs1, std::vector<AABB> aabbs2, int& i, int& j)
{

	for(; i < aabbs1.size(); ++i)
	{
		AABB aabb1{aabbs1[i]};

		for (; j < aabbs2.size(); ++j)
		{
			AABB aabb2{ aabbs2[j] };

			if (AreIntervalsOverlapping(aabb1.min.x, aabb2.min.x, aabb1.max.x, aabb2.max.x) &&
				AreIntervalsOverlapping(aabb1.min.y, aabb2.min.y, aabb1.max.y, aabb2.max.y) &&
				AreIntervalsOverlapping(aabb1.min.z, aabb2.min.z, aabb1.max.z, aabb2.max.z)
				)
			{
				++j;
				return { true, std::pair<AABB, AABB>(aabb1, aabb2) };
			}
		}
		j = 0;
	}
	return CollisionInfo{};
}

CollisionInfo CollisionFixer::AreColliding(CollisionComponent* col1, CollisionComponent* col2)
{
	std::vector<AABB> aabbs1{ col1->GetAABBs() };
	std::vector<AABB> aabbs2{ col2->GetAABBs() };
	int i{};
	int j{};

	return AreColliding(aabbs1, aabbs2, i, j);
}
CollisionInfo CollisionFixer::AreColliding(CollisionComponent* col1, CollisionComponent* col2, int& i, int& j)
{
	std::vector<AABB> aabbs1{ col1->GetAABBs() };
	std::vector<AABB> aabbs2{ col2->GetAABBs() }; 
	
	return AreColliding(aabbs1, aabbs2, i, j);
}
CollisionInfo CollisionFixer::AreColliding(CollisionComponent* col, std::vector<AABB> aabbs2)
{
	std::vector<AABB> aabbs1{ col->GetAABBs() };
	int i{};
	int j{};

	return AreColliding(aabbs1, aabbs2, i, j);
}

bool CollisionFixer::AreBothStaticMeshes(CollisionComponent* col1, CollisionComponent* col2)
{
	return col1->HasStaticCollision() && col2->HasStaticCollision();
}

bool CollisionFixer::AreBothNonStaticMeshes(CollisionComponent* col1, CollisionComponent* col2)
{
	return !col1->HasStaticCollision() && !col2->HasStaticCollision();
}

bool CollisionFixer::AreIntervalsOverlapping(float a1, float a2, float A1, float A2)
{
	return
		a2 <= A1 &&
		a1 <= A2;
}

std::pair<glm::vec3, glm::vec3> CollisionFixer::CalculateCollisionDistances(AABB aabb1, AABB aabb2)
{
	std::pair<glm::vec3, glm::vec3> distances{};

	glm::vec3 min1{ aabb1.min };
	glm::vec3 min2{ aabb2.min };
	glm::vec3 max1{ aabb1.max };
	glm::vec3 max2{ aabb2.max };

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
