#pragma once

#include <vector>

#include "../Mesh.h"

class Scene
{
public:

	void AddMesh(Mesh* mesh)
	{
		m_Meshes.emplace_back(mesh);
	}

private:

	std::vector<Mesh*> m_Meshes{};

};