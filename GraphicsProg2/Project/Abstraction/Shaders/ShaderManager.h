#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../Singleton.h"
#include "Shader.h"

class ShaderManager : public Singleton<ShaderManager>
{
public:

	// returns the index of the shader
	[[nodiscard]] uint32_t AddShader(Shader* shader, const VkDevice& device)
	{
		m_Shaders.emplace_back(shader);
		shader->Initialize(device);
		return static_cast<uint32_t>(m_Shaders.size() - 1);
	}

	std::vector<Shader*> GetShaders()
	{
		std::vector<Shader*> shaders{};
		for (auto& shader : m_Shaders)
		{
			shaders.emplace_back(shader.get());
		}
		return shaders;
	}

private:

	std::vector<std::unique_ptr<Shader>> m_Shaders{};

};