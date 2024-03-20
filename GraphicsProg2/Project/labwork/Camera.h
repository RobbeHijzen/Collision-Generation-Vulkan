#pragma once
#include <cassert>
#include "vulkanbase/VulkanUtil.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>

struct Camera
{

	Camera(const glm::vec3& _origin, float _fovAngle) :
		origin{ _origin },
		fovAngle{ _fovAngle }
	{
		fov = tanf(glm::radians(fovAngle / 2.f));

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
	//float totalYaw{3.14159265f / 2.f};
	float totalYaw{0.f};

	glm::mat4 invViewMatrix{};
	glm::mat4 viewMatrix{};

	glm::mat4 projectionMatrix{};

	const float nearDistance{ 0.1f };
	const float farDistance{ 10.f };

	const float m_MoveSpeed{ 40.f };
	const float m_MouseMoveMultiplier{ 10.f };
	const float m_PanSpeed{ 1.f / 180.f };

	void CalculateViewMatrix()
	{
		const glm::mat4 finalRotation{ glm::rotate(unitMatrix, totalPitch, glm::vec3{1.f, 0.f, 0.f})
									  * glm::rotate(unitMatrix, totalYaw, glm::vec3{0.f, 1.f, 0.f}) };

		glm::vec4 forwardVec4{ finalRotation * glm::vec4{ 0.f, 0.f, 1.f, 0.f} };
		forward = { forwardVec4.x, forwardVec4.y, forwardVec4.z};
		right = (glm::cross({ 0.f, 1.f, 0.f }, forward));
		up = (glm::cross(forward, right));
		
		
		viewMatrix = glm::lookAt(origin, forward - origin, up);
	}

	void CalculateProjectionMatrix()
	{
		float width{ WIDTH };
		float height{ HEIGHT };

		projectionMatrix = glm::perspective(fov, width / height, nearDistance, farDistance);
	}


	void Update(float deltaTime)
	{
		CalculateViewMatrix();

		MovementInputs(deltaTime);
	}
	void MovementInputs(float deltaTime)
	{
		//const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
		//
		//if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
		//{
		//	origin += m_MoveSpeed * deltaTime * forward;
		//
		//}
		//if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
		//{
		//	origin -= m_MoveSpeed * deltaTime * right;
		//
		//}
		//if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
		//{
		//	origin -= m_MoveSpeed * deltaTime * forward;
		//
		//}
		//if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
		//{
		//	origin += m_MoveSpeed * deltaTime * right;
		//
		//}
		//if (pKeyboardState[SDL_SCANCODE_E])
		//{
		//	origin.y += m_MoveSpeed * deltaTime;
		//
		//}
		//if (pKeyboardState[SDL_SCANCODE_Q])
		//{
		//	origin.y -= m_MoveSpeed * deltaTime;
		//
		//}
		//
		//
		////Mouse Input
		//
		//int mouseX{}, mouseY{};
		//const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
		//
		//// Left AND Right MouseButton
		//if (mouseState & SDL_BUTTON(1) && mouseState & SDL_BUTTON(3))
		//{
		//	origin += -mouseY * deltaTime * up * m_MouseMoveMultiplier;
		//}
		//// Left MouseButton
		//else if (mouseState & SDL_BUTTON(1))
		//{
		//	origin += -mouseY * deltaTime * forward * m_MouseMoveMultiplier;
		//	totalYaw += mouseX * m_PanSpeed;
		//}
		//// Right MouseButton
		//else if (mouseState & SDL_BUTTON(3))
		//{
		//	totalYaw += mouseX * m_PanSpeed;
		//	totalPitch += mouseY * m_PanSpeed;
		//}
	}
};

