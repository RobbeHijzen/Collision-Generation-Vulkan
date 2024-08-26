#pragma once
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Vulkanbase/Helperstructs/BaseComponent.h"
#include "PhysicsGame/Meshes/Mesh.h"


class MovementComponent : public BaseComponent
{
public:

	MovementComponent(Mesh* pParent) : BaseComponent(pParent) {}
	virtual void Update(GLFWwindow* window) override;

private:

	glm::vec3 m_Velocity{};

	//----------------
	// Horizontal Movement
	//----------------
	const float m_MoveSpeed{ 250.f };
	const float m_RotationSpeed{ 1.f / 180.f };

	int m_LastMouseX = 0;

	void HandleKeyboardMovement(GLFWwindow* window);
	void HandleMouseMovement(GLFWwindow* window);

	//----------------
	// Vertical Movement
	//----------------
	bool m_IsOnGround{ true };
	bool m_CanJump{ true };

	float m_JumpStrength{ 700.f };
	float m_Gravity{ 981.f };
	//float m_Gravity{ 0.f };

	void HandleGravity();
	void CalculateIsOnGround();
};
