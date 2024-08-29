#include "vulkanbase/VulkanBase.h"

void VulkanBase::LoadScene()
{
	// Plane
	auto plane{ new Mesh("Resources/objs/Plane.obj", "Resources/texs/Plane_BaseColor.png") };
	plane->AddComponent(std::make_shared<CollisionComponent>(plane, true, 0));
	m_Scene->AddObject(plane);

	// Vehicle
	auto vehicle{ new Mesh("Resources/objs/vehicle.obj", "Resources/texs/vehicle_diffuse.png", { -200, 82.f, 0 }, { 0, -90, 0 }) };
	vehicle->AddComponent(std::make_shared<CollisionComponent>(vehicle, true, 4));
	m_Scene->AddObject(vehicle);

	// Manny
	auto manny{ new Mesh("Resources/objs/Manny.obj", "Resources/texs/Manny_BaseColor.png", {0.f, 150.f, 0.f}) };

	manny->AddComponent(std::make_shared<CollisionComponent>(manny, false, 0));
	manny->AddComponent(std::make_shared<MovementComponent>(manny));
	manny->AddComponent(std::make_shared<CameraComponent>(manny, m_Camera.get()));
	
	m_Scene->AddObject(manny);
}

void VulkanBase::HandleToggleKeyboardPresses(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && m_CanToggleDrawOutlines)
	{
		m_CanToggleDrawOutlines = false;
		m_DrawOutlines = !m_DrawOutlines;

		for (auto& object : m_Scene->GetObjects())
		{
			if (auto col = object->GetComponent<CollisionComponent>())
			{
				col->SetHidden(!m_DrawOutlines);
			}
		}
	}
	else if(glfwGetKey(window, GLFW_KEY_E) != GLFW_PRESS)
	{
		m_CanToggleDrawOutlines = true;
	}

}
