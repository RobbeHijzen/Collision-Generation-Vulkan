#include "PlayerMesh.h"

PlayerMesh::PlayerMesh(std::string objPath, std::string diffuseString, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
	: Mesh(objPath, diffuseString, translation, rotation, scale)
{
}

void PlayerMesh::GameStart()
{
    HandleCameraMovement();
}

void PlayerMesh::Update(float deltaTime, GLFWwindow* window)
{
	MovementInputs(deltaTime, window);
	HandleCameraMovement();
}

void PlayerMesh::Translate(glm::vec3 addedPos)
{
    Mesh::Translate(addedPos);
    HandleCameraMovement();
}

void PlayerMesh::Rotate(glm::vec3 addedRot)
{
    Mesh::Rotate(addedRot);
    HandleCameraMovement();
}

void PlayerMesh::Scale(glm::vec3 addedScale)
{
    Mesh::Scale(addedScale);
}

void PlayerMesh::HandleCameraMovement()
{
    if (m_Camera)
    {
		m_Camera->origin = m_WorldPos + RotateVectorY(m_RelativeCameraPos, m_WorldRot.y);
        m_Camera->totalYaw = m_WorldRot.y;
    }
}

void PlayerMesh::MovementInputs(float deltaTime, GLFWwindow* window)
{
	glm::vec3 forward{ RotateVectorY(glm::vec3{0.f, 0.f, 1.f}, m_WorldRot.y)};
	glm::vec3 right{ RotateVectorY(glm::vec3{1.f, 0.f, 0.f}, m_WorldRot.y) };

	// Keyboard input
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		Translate(m_MoveSpeed * deltaTime * forward);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		Translate(m_MoveSpeed * deltaTime * right);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		Translate(-m_MoveSpeed * deltaTime * forward);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		Translate(-m_MoveSpeed * deltaTime * right);
	}

	// Mouse Input
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	int mouseX{ static_cast<int>(x) - m_LastMouseX };
	int mouseY{ static_cast<int>(y) - m_LastMouseY };
	m_LastMouseX = static_cast<int>(x);
	m_LastMouseY = static_cast<int>(y);

	int LMBState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int RMBState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	// Mouse Movement
	if (RMBState == GLFW_PRESS)
	{
		Rotate({0.f, mouseX * m_PanSpeed, 0.f});
		m_Camera->totalPitch += mouseY * m_PanSpeed;
	}

	m_Camera->totalPitch = std::clamp(m_Camera->totalPitch, -PI / 2.f, PI / 2.f);
}

glm::vec3 PlayerMesh::RotateVectorY(glm::vec3 vec, float angle)
{
	glm::vec3 finalVec{};
	finalVec.x = -(vec.x * cosf(angle) - vec.z * sin(angle));
	finalVec.y = vec.y;
	finalVec.z = vec.x * sinf(angle) + vec.z * cos(angle);

	return finalVec;
}
