#include "vulkanbase/VulkanBase.h"

#include "Abstraction/Components/DerivedComponents/CameraComponent.h"
#include "Abstraction/Components/DerivedComponents/CollisionComponent.h"
#include "Abstraction/Components/DerivedComponents/MovementComponent.h"

void VulkanBase::LoadScene()
{
	// Gun
	auto gun{ new Mesh("Resources/xm177.obj", "Resources/xm177_basecolor.png", { 200.f, 50.f, 250.f }, {}) };
	gun->AddComponent(std::make_shared<CollisionComponent>(gun, false));
	m_Scene->AddMesh(gun);

	// Vehicle
	auto vehicle{ new Mesh("Resources/vehicle.obj", "Resources/vehicle_diffuse.png", { -200, 85.f, 0 }, { 0, -90, 0 }) };
	vehicle->AddComponent(std::make_shared<CollisionComponent>(vehicle, true));
	m_Scene->AddMesh(vehicle);

	// Plane
	auto plane{ new Mesh("Resources/Plane.obj", "Resources/Plane_BaseColor.png") };
	plane->AddComponent(std::make_shared<CollisionComponent>(plane, true));
	m_Scene->AddMesh(plane);

	// Manny
	auto manny{ new Mesh("Resources/Manny.obj", "Resources/Manny_BaseColor.png") };

	manny->AddComponent(std::make_shared<CollisionComponent>(manny, false));
	manny->AddComponent(std::make_shared<MovementComponent>(manny));
	manny->AddComponent(std::make_shared<CameraComponent>(manny, m_Camera.get()));

	m_Scene->AddMesh(manny);


	// Game Start
	for (auto mesh : m_Scene->GetMeshes())
	{
		mesh->GameStart();
	}
}