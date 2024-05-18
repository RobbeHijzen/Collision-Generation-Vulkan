#pragma once

#include <vector>
#include <memory>

#include "Abstraction/Meshes/Mesh.h"
#include "../Singleton.h"

class Scene : public Singleton<Scene>
{
public:
	Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
	Scene(Scene&& other) noexcept = delete;
	Scene& operator=(Scene&& other) noexcept = delete;

	void GameStart()
	{
		for (const auto& mesh : m_Meshes)
		{
			mesh->GameStart();
		}
	}
	void Update(GLFWwindow* window)
	{
		for (const auto& mesh : m_Meshes)
		{
			mesh->Update(window);
		}
	}
	void LateUpdate()
	{
		for (const auto& mesh : m_Meshes)
		{
			mesh->LateUpdate();
		}
	}



	void AddMesh(Mesh* mesh)
	{
		mesh->SetMeshIndex(static_cast<uint32_t>(m_Meshes.size()));
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

	auto GetMeshesAmount() const { return m_Meshes.size(); }

private:

	std::vector<std::unique_ptr<Mesh>> m_Meshes{};

};