#pragma once

#include <glm/glm.hpp>

static glm::vec3 RotateVectorY(glm::vec3 vec, float angle)
{
	glm::vec3 finalVec{};
	finalVec.x = -(vec.x * cosf(angle) - vec.z * sin(angle));
	finalVec.y = vec.y;
	finalVec.z = vec.x * sinf(angle) + vec.z * cos(angle);

	return finalVec;
}
