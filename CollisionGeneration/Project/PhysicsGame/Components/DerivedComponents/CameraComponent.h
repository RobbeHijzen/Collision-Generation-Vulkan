#pragma once

#include "Vulkanbase/Helperstructs/BaseComponent.h"

struct Camera;
class CameraComponent : public BaseComponent
{
public:

	CameraComponent(Mesh* pParent, Camera* pCamera, float panSpeed = 1.f / 180.f);

	virtual void Update(GLFWwindow* window) override;
	void HandleCameraMovement();

private:

	Camera* m_Camera{};
	glm::vec3 m_RelativeCameraPos{ 0.f, 240.f, -100.f };

	float m_PanSpeed{ 1.f / 180.f };
	int m_LastMouseY = 0;

	void HandleMouseMovement(GLFWwindow* window);
};
