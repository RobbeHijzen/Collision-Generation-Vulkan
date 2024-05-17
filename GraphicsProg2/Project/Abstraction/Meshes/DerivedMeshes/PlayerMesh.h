#include "Abstraction/Meshes/Mesh.h"

#include "vulkan/vulkan_core.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>
#include <optional>

#include "Abstraction/VertexInfo.h"
#include "Abstraction/Camera.h"

class PlayerMesh : public Mesh
{
public:
	PlayerMesh(std::string objPath,
		std::string diffuseString,
		glm::vec3 translation = glm::vec3{ 0.f, 0.f, 0.f },
		glm::vec3 rotation = glm::vec3{ 0.f, 0.f, 0.f },
		glm::vec3 scale = glm::vec3{ 1.f, 1.f, 1.f });

	virtual void GameStart() override;
	virtual void Update(float deltaTime, GLFWwindow* window) override;

	void AttachCamera(Camera* camera) { m_Camera = camera; }

	virtual void Translate(glm::vec3 addedPos) override;
	virtual void Rotate(glm::vec3 addedRot) override;
	virtual void Scale(glm::vec3 addedScale) override;

private:

	Camera* m_Camera{};
	glm::vec3 m_RelativeCameraPos{ 0.f, 25.f, -10.f };


	const float m_MoveSpeed{ 25.f };
	const float m_PanSpeed{ 1.f / 180.f };

	int m_LastMouseX = 0;
	int m_LastMouseY = 0;

	void HandleCameraMovement();
	void MovementInputs(float deltaTime, GLFWwindow* window);

	glm::vec3 RotateVectorY(glm::vec3 vec, float angle);
};