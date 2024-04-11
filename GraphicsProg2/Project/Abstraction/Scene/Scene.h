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


	void AddMesh(Mesh* mesh)
	{
		m_Meshes.emplace_back(mesh);
	}

	std::vector<Mesh*> GetMeshes()
	{
		std::vector<Mesh*> meshes{};
		for (const auto& mesh : m_Meshes)
		{
			meshes.emplace_back(mesh.get());
		}
		return meshes;
	}

private:

	std::vector<std::unique_ptr<Mesh>> m_Meshes{};

};