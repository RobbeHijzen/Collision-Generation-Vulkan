#pragma once

#include <vector>
#include <memory>

#include "../Mesh.h"

class Scene
{
public:
	Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
	Scene(Scene&& other) noexcept = delete;
	Scene& operator=(Scene&& other) noexcept = delete;


	void AddMesh(Mesh3D* mesh)
	{
		m_Meshes.emplace_back(mesh);
	}

	std::vector<Mesh3D*> GetMeshes()
	{
		std::vector<Mesh3D*> meshes{};
		for (const auto& mesh : m_Meshes)
		{
			meshes.emplace_back(mesh.get());
		}
		return meshes;
	}

	auto GetMeshesAmount() const { return m_Meshes.size(); }

private:

	std::vector<std::unique_ptr<Mesh3D>> m_Meshes{};

};