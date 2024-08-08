#include "vulkanbase/VulkanBase.h"

void VulkanBase::LoadScene()
{
	// Plane
	auto plane{ new Mesh(0, false, "Resources/objs/Plane.obj", "Resources/texs/Plane_BaseColor.png") };
	plane->AddComponent(std::make_shared<CollisionComponent>(plane, true, 0));
	m_Scene->AddMesh(plane);

	// Vehicle
	auto vehicle{ new Mesh(1, false, "Resources/objs/vehicle.obj", "Resources/texs/vehicle_diffuse.png", { -200, 82.f, 0 }, { 0, -90, 0 }) };
	vehicle->AddComponent(std::make_shared<CollisionComponent>(vehicle, true, 2));
	m_Scene->AddMesh(vehicle);
	
	// Environment
	auto environment{ new Mesh(2, true, "Resources/objs/Environment.obj", "Resources/texs/null_BaseColor.png", {-3000.f, -549.f, 0.f}, {0.f, 90.f, 0.f}) };
	environment->AddComponent(std::make_shared<CollisionComponent>(environment, true, 5));
	m_Scene->AddMesh(environment);

	// Manny
	auto manny{ new Mesh(3, false, "Resources/objs/Manny.obj", "Resources/texs/Manny_BaseColor.png", {0.f, 150.f, 0.f}) };

	manny->AddComponent(std::make_shared<CollisionComponent>(manny, false, 0));
	manny->AddComponent(std::make_shared<MovementComponent>(manny));
	manny->AddComponent(std::make_shared<CameraComponent>(manny, m_Camera.get()));
	
	m_Scene->AddMesh(manny);
}

void VulkanBase::HandleToggleKeyboardPresses(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && m_CanToggleDrawOutlines)
	{
		m_CanToggleDrawOutlines = false;
		m_DrawOutlines = !m_DrawOutlines;
	}
	else if(glfwGetKey(window, GLFW_KEY_E) != GLFW_PRESS)
	{
		m_CanToggleDrawOutlines = true;
	}

}
