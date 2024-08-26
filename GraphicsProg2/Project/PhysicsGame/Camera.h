#pragma once
#include <cassert>
#include "VulkanUtil/VulkanUtil.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


struct Camera
{

	Camera(const glm::vec3& _origin, float _fovAngle) :
		origin{ _origin },
		fovAngle{ _fovAngle }
	{
		fov = glm::radians(fovAngle / 2.f);

		CalculateProjectionMatrix();
	}

	glm::vec3 origin;
	float fovAngle;
	float fov;
	
	glm::mat4 unitMatrix{ glm::mat4{1.f} };

	glm::vec3 forward{ 0.f, 0.f, 1.f };
	glm::vec3 up{ 0.f, 1.f, 0.f };
	glm::vec3 right{ 1.f, 0.f, 0.f };

	float totalPitch{ glm::radians(40.f) };
	float totalYaw{0.f};

	glm::mat4 invViewMatrix{};
	glm::mat4 viewMatrix{};

	glm::mat4 projectionMatrix{};

	const float nearDistance{ 0.1f };
	const float farDistance{ 100000.f };

	void CalculateViewMatrix()
	{
		const glm::mat4 finalRotation{   glm::rotate(unitMatrix, totalPitch, glm::vec3{1.f, 0.f, 0.f})
									    * glm::rotate(unitMatrix,  totalYaw, glm::vec3{0.f, 1.f, 0.f}) };
		const glm::mat4 finalTranslation{ glm::translate(unitMatrix, glm::vec3{-origin.x, -origin.y, origin.z} ) };

		auto inverseRot{glm::inverse(finalRotation)};
		glm::vec4 transformedForward = inverseRot * glm::vec4(0.f, 0.f, 1.f, 0.f);

		forward = glm::normalize(glm::vec3{ -transformedForward.x, -transformedForward.y, transformedForward.z });
		right = glm::normalize(glm::cross({ 0.f, 1.f, 0.f }, forward));
		up = glm::normalize(glm::cross(forward, right));

		viewMatrix = finalRotation * finalTranslation;
	}

	void CalculateProjectionMatrix()
	{
		float width{ WIDTH };
		float height{ HEIGHT };

		projectionMatrix = glm::perspective(fov, width / height, nearDistance, farDistance);
		projectionMatrix[1][1] *= -1;
	}


	void Update(GLFWwindow* window)
	{
		CalculateViewMatrix();
	}
};

