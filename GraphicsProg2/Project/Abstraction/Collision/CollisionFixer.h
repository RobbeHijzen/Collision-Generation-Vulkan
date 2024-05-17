#include "Abstraction/Meshes/Mesh.h"

class CollisionFixer
{
public:

	static void FixCollisions(std::vector<Mesh*> meshes);

private:

	static void HandleCollision(Mesh* mesh1, Mesh* mesh2);
	static bool AreColliding(Mesh* mesh1, Mesh* mesh2);

	static bool AreBothStaticMeshes(Mesh* mesh1, Mesh* mesh2);
	static bool AreBothNonStaticMeshes(Mesh* mesh1, Mesh* mesh2);

	static bool AreIntervalsOverlapping(float a1, float a2, float A1, float A2);

	static std::pair<glm::vec3, glm::vec3> CalculateCollisionDistances(Mesh* mesh1, Mesh* mesh2);
};