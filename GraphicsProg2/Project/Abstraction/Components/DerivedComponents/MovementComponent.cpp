#include "MovementComponent.h"
#include "Abstraction/MathHelpers.h"
#include "Abstraction/Scene/Scene.h"
#include "Abstraction/Collision/CollisionFixer.h"
#include "Abstraction/Components/DerivedComponents/CollisionComponent.h"

#include <algorithm>
#include <vector>

void MovementComponent::Update(float deltaTime, GLFWwindow* window)
{
	CalculateIsOnGround();
	std::cout << "IsOnGround: " << m_IsOnGround << "\n";

	HandleKeyboardMovement(window);
	HandleGravity(deltaTime);

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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (m_IsOnGround)
		{
			newVelocity.y += m_JumpStrength;
			m_IsOnGround = false;
		}
	}
	
	m_Velocity.x = newVelocity.x;
	m_Velocity.y += newVelocity.y;
	m_Velocity.z = newVelocity.z;

	
}

void MovementComponent::HandleGravity(float deltaTime)
{
	if (!m_IsOnGround)
	{
		m_Velocity -= m_Gravity * deltaTime;
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
	auto meshes{ Scene::GetInstance()->GetMeshes() };
	std::erase(meshes, GetOwner());

	auto collisionComp{ GetOwner()->GetComponent<CollisionComponent>() };
	if (!collisionComp) return;

	glm::vec3 min{ collisionComp->GetMinAABB() };
	glm::vec3 max{ collisionComp->GetMaxAABB() };
	
	min.x += FLT_EPSILON * 6;
	max.x -= FLT_EPSILON * 6;
	
	max.y = min.y + FLT_EPSILON;
	min.y -= FLT_EPSILON * 80;

	min.z += FLT_EPSILON * 6;
	max.z -= FLT_EPSILON * 6;

	for (auto mesh : meshes)
	{
		if (auto col = mesh->GetComponent<CollisionComponent>())
		{
			if (CollisionFixer::AreColliding(col.get(), min, max))
			{
				m_IsOnGround = true;
				m_Velocity.y = 0.f;
				return;
			}
		}
	}

	m_IsOnGround = false;
}

