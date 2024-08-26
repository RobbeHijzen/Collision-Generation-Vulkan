#include "MovementComponent.h"
#include "PhysicsGame/MathHelpers.h"
#include "Vulkanbase/Scene/Scene.h"
#include "PhysicsGame/Collision/CollisionFixer.h"
#include "PhysicsGame/Components/DerivedComponents/CollisionComponent.h"
#include "Vulkanbase/Time/Time.h"

#include <algorithm>
#include <vector>

void MovementComponent::Update(GLFWwindow* window)
{
	CalculateIsOnGround();
	//std::cout << "IsOnGround: " << m_IsOnGround << "\n";

	HandleKeyboardMovement(window);
	HandleGravity();

	HandleMouseMovement(window);

	GetOwner()->SetVelocity(m_Velocity);
}

void MovementComponent::HandleKeyboardMovement(GLFWwindow* window)
{
	glm::vec3 worldRot{ GetOwner()->GetWorldRotation() };

	glm::vec3 forward{ RotateVectorY(glm::vec3{0.f, 0.f, 1.f}, worldRot.y) };
	glm::vec3 right{ RotateVectorY(glm::vec3{1.f, 0.f, 0.f}, worldRot.y) };

	// Keyboard input
	glm::vec3 newVelocity{};

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		newVelocity += m_MoveSpeed * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		newVelocity += m_MoveSpeed * right;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		newVelocity += -m_MoveSpeed * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		newVelocity += -m_MoveSpeed * right;
	}

	bool jumpKeyDown{ glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS };
	if (jumpKeyDown && m_CanJump)
	{
		m_CanJump = false;
		if (m_IsOnGround)
		{
			newVelocity.y += m_JumpStrength;
			m_IsOnGround = false;
		}
	}
	else if (!jumpKeyDown) m_CanJump = true;
	
	m_Velocity.x = newVelocity.x;
	m_Velocity.y += newVelocity.y;
	m_Velocity.z = newVelocity.z;

	
}

void MovementComponent::HandleGravity()
{
	if (!m_IsOnGround)
	{
		m_Velocity.y -= m_Gravity * Time::GetInstance()->GetDeltaTime();
	}
}

void MovementComponent::HandleMouseMovement(GLFWwindow* window)
{
	// X Mouse Input
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	int mouseX{ static_cast<int>(x) - m_LastMouseX };
	m_LastMouseX = static_cast<int>(x);

	// Mouse Movement
	GetOwner()->Rotate({ 0.f, mouseX * m_RotationSpeed, 0.f });
}

void MovementComponent::CalculateIsOnGround()
{
	auto meshes{ Scene::GetInstance()->GetObjects() };
	std::erase(meshes, GetOwner());

	auto collisionComp{ GetOwner()->GetComponent<CollisionComponent>() };
	if (!collisionComp) return;

	if (CollisionFixer::IsOnGround(collisionComp.get(), meshes))
	{
		m_IsOnGround = true;
		if(m_Velocity.y < 0)
			m_Velocity.y = 0.f;
	}
	else
	{
		m_IsOnGround = false;
	}
}

