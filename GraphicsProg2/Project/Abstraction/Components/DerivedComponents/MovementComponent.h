#pragma once
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Abstraction/Components/BaseComponent.h"
#include "Abstraction/Meshes/Mesh.h"


class MovementComponent : public BaseComponent
{
public:

	MovementComponent(Mesh* pParent) : BaseComponent(pParent) {}
	virtual void Update(float deltaTime, GLFWwindow* window) override;

private:

	glm::vec3 m_Velocity{};

	//----------------
	// Horizontal Movement
	//----------------
	const float m_MoveSpeed{ 250.f };
	const float m_RotationSpeed{ 1.f / 180.f };

	int m_LastMouseX = 0;

	//----------------
	// Vertical Movement
	//----------------
	bool m_IsOnGround{ true };

	float m_JumpStrength{ 1000.f };
	float m_Gravity{ 981.f };


	void HandleKeyboardMovement(GLFWwindow* window);
	void HandleGravity(float deltaTime);
	void HandleMouseMovement(GLFWwindow* window);

	void CalculateIsOnGround();
};
