#include "CameraComponent.h"
#include "PhysicsGame/MathHelpers.h"
#include "PhysicsGame/Meshes/Mesh.h"

CameraComponent::CameraComponent(Mesh* pParent, Camera* pCamera, float panSpeed)
	: BaseComponent(pParent)
	, m_Camera{pCamera}
	, m_PanSpeed{panSpeed}
{
	Observer* obs{ new Observer(GameEvents::ModelMatrixChanged, [&] { this->HandleCameraMovement(); }) };
	pParent->AddObserver(obs);
}

void CameraComponent::Update(GLFWwindow* window)
{
	HandleMouseMovement(window);
}

void CameraComponent::HandleCameraMovement()
{
	if (m_Camera)
	{
		auto worldPos{GetOwner()->GetWorldPosition()};
		auto worldRot{GetOwner()->GetWorldRotation()};

		m_Camera->origin = worldPos + RotateVectorY(m_RelativeCameraPos, worldRot.y);
		m_Camera->totalYaw = worldRot.y;
	}
}

void CameraComponent::HandleMouseMovement(GLFWwindow* window)
{
	// Y Mouse Input
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	int mouseY{ static_cast<int>(y) - m_LastMouseY };
	m_LastMouseY = static_cast<int>(y);
	
	m_Camera->totalPitch += mouseY * m_PanSpeed;
	m_Camera->totalPitch = std::clamp(m_Camera->totalPitch, -PI / 10.f, PI / 4.f);
}


