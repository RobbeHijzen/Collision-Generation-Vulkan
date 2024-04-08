#pragma once
#include <cassert>
#include "vulkanbase/VulkanUtil.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

	float totalPitch{0.f};
	float totalYaw{0.f};

	glm::mat4 invViewMatrix{};
	glm::mat4 viewMatrix{};

	glm::mat4 projectionMatrix{};

	const float nearDistance{ 0.1f };
	const float farDistance{ 10.f };

	const float m_MoveSpeed{ 10.f };
	const float m_MouseMoveMultiplier{ 10.f };
	const float m_PanSpeed{ 0.45f };

	int m_LastMouseX = 0;
	int m_LastMouseY = 0;

	void CalculateViewMatrix()
	{
		const glm::mat4 finalRotation{ glm::rotate(unitMatrix, glm::radians(totalPitch), glm::vec3{1.f, 0.f, 0.f})
									  * glm::rotate(unitMatrix,  glm::radians(totalYaw), glm::vec3{0.f, 1.f, 0.f}) };

		glm::vec4 forwardVec4{ finalRotation * glm::vec4{ 0.f, 0.f, 1.f, 0.f} };
		forward = { forwardVec4.x, forwardVec4.y, forwardVec4.z};
		right = (glm::cross({ 0.f, 1.f, 0.f }, -forward));
		up = (glm::cross(-forward, right));
		
		
		viewMatrix = glm::lookAt(origin, forward + origin, up);
	}

	void CalculateProjectionMatrix()
	{
		float width{ WIDTH };
		float height{ HEIGHT };

		projectionMatrix = glm::perspective(fov, width / height, nearDistance, farDistance);
	}


	void Update(float deltaTime, GLFWwindow* window)
	{
		CalculateViewMatrix();

		MovementInputs(deltaTime, window);
	}
	void MovementInputs(float deltaTime, GLFWwindow* window)
	{
		// Keyboard input
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
		{
			origin += m_MoveSpeed * deltaTime * forward;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
		{
			origin -= m_MoveSpeed * deltaTime * right;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
		{
			origin -= m_MoveSpeed * deltaTime * forward;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) 
		{
			origin += m_MoveSpeed * deltaTime * right;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) 
		{
			origin.y += m_MoveSpeed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
		{
			origin.y -= m_MoveSpeed * deltaTime;
		}

		// Mouse Input
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		int mouseX{ static_cast<int>(x) - m_LastMouseX};
		int mouseY{ static_cast<int>(y) - m_LastMouseY};
		m_LastMouseX = static_cast<int>(x);
		m_LastMouseY = static_cast<int>(y);

		int LMBState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		int RMBState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

		// Left AND Right MouseButton
		if (LMBState == GLFW_PRESS && RMBState == GLFW_PRESS)
		{
			origin += -mouseY * deltaTime * up * m_MouseMoveMultiplier;
		}
		// Left MouseButton
		else if (LMBState == GLFW_PRESS) 
		{
			origin += -mouseY * deltaTime * forward * m_MouseMoveMultiplier;
			totalYaw -= mouseX * m_PanSpeed;
		}
		// Right MouseButton
		else if (RMBState == GLFW_PRESS)
		{
			totalYaw -= mouseX * m_PanSpeed;
			totalPitch -= mouseY * m_PanSpeed;
		}
	}
};

