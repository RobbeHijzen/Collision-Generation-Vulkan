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
		m_Meshes3D.emplace_back(mesh);
	}
	void AddMesh(Mesh2D* mesh)
	{
		m_Meshes2D.emplace_back(mesh);
	}

	std::vector<Mesh3D*> GetMeshes3D()
	{
		std::vector<Mesh3D*> meshes{};
		for (const auto& mesh : m_Meshes3D)
		{
			meshes.emplace_back(mesh.get());
		}
		return meshes;
	}
	std::vector<Mesh2D*> GetMeshes2D()
	{
		std::vector<Mesh2D*> meshes{};
		for (const auto& mesh : m_Meshes2D)
		{
			meshes.emplace_back(mesh.get());
		}
		return meshes;
	}
	std::vector<Mesh*> GetAllMeshes()
	{
		std::vector<Mesh*> meshes{};
		for (const auto& mesh : m_Meshes3D)
		{
			meshes.emplace_back(mesh.get());
		}
		for (const auto& mesh : m_Meshes2D)
		{
			meshes.emplace_back(mesh.get());
		}

		return meshes;
	}

	uint32_t GetAmount3DMeshes() const { return m_Meshes3D.size(); }
	uint32_t GetAmount2DMeshes() const { return m_Meshes2D.size(); }
	uint32_t GetAmountMeshes() const { return GetAmount3DMeshes() + GetAmount2DMeshes(); }

private:

	std::vector<std::unique_ptr<Mesh3D>> m_Meshes3D{};
	std::vector<std::unique_ptr<Mesh2D>> m_Meshes2D{};

};