#pragma once

#include "Abstraction/Components/BaseComponent.h"

struct Camera;
class CameraComponent : public BaseComponent
{
public:

	CameraComponent(Mesh* pParent, Camera* pCamera, float panSpeed = 1.f / 180.f);

	virtual void Update(float deltaTime, GLFWwindow* window) override;
	void HandleCameraMovement();

private:

	Camera* m_Camera{};
	glm::vec3 m_RelativeCameraPos{ 0.f, 240.f, -100.f };

	float m_PanSpeed{ 1.f / 180.f };
	int m_LastMouseY = 0;

	void HandleMouseMovement(float deltaTime, GLFWwindow* window);
};
