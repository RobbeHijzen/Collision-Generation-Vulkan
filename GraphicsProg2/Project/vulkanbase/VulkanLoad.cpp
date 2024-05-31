#include "vulkanbase/VulkanBase.h"

void VulkanBase::LoadScene()
{
	// Gun
	//auto gun{ new Mesh("Resources/xm177.obj", "Resources/xm177_basecolor.png", { 200.f, 50.f, 250.f }, {}) };
	//gun->AddComponent(std::make_shared<CollisionComponent>(gun, false));
	//m_Scene->AddMesh(gun);

	// Vehicle
	auto vehicle{ new Mesh(0, false, "Resources/objs/vehicle.obj", "Resources/texs/vehicle_diffuse.png", { -200, 82.f, 0 }, { 0, -90, 0 }) };
	vehicle->AddComponent(std::make_shared<CollisionComponent>(vehicle, true, 5));
	m_Scene->AddMesh(vehicle);
	
	// Plane
	//auto plane{ new Mesh(1, "Resources/objs/Plane.obj", "Resources/texs/Plane_BaseColor.png") };
	//plane->AddComponent(std::make_shared<CollisionComponent>(plane, true, 1));
	//m_Scene->AddMesh(plane);

	// Environment
	auto environment{ new Mesh(1, true, "Resources/objs/environment.obj", "Resources/texs/null_BaseColor.png") };
	environment->AddComponent(std::make_shared<CollisionComponent>(environment, true, 2000));
	m_Scene->AddMesh(environment);

	// Manny
	auto manny{ new Mesh(2, false, "Resources/objs/Manny.obj", "Resources/texs/Manny_BaseColor.png", {0.f, 750.f, 0.f}) };

	manny->AddComponent(std::make_shared<CollisionComponent>(manny, false, 25));
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
