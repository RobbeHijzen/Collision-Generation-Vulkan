#pragma once
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>

class Mesh;

class BaseComponent
{
public:

	virtual ~BaseComponent() = default;
	BaseComponent(const BaseComponent& other) = delete;
	BaseComponent(BaseComponent&& other) = delete;
	BaseComponent& operator=(const BaseComponent& other) = delete;
	BaseComponent& operator=(BaseComponent&& other) = delete;

	virtual void GameStart() {};
	virtual void Update(GLFWwindow* window) {};
	virtual void LateUpdate() {};

	Mesh* GetOwner() const { return m_Owner; }

protected:

	BaseComponent(Mesh* pParent) : m_Owner{pParent} {}

private:

	Mesh* m_Owner;
};