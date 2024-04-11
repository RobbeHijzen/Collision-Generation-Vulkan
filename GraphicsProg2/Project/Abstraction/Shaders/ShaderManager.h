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
	[[nodiscard]] uint32_t AddShader(Shader* shader)
	{
		m_Shaders.emplace_back(shader);
		return static_cast<uint32_t>(m_Shaders.size() - 1);
	}

	Shader* GetShader(uint32_t index) const { return m_Shaders[index].get(); }
	uint32_t GetShaderSize() const { return static_cast<uint32_t>(m_Shaders.size()); }

private:

	std::vector<std::unique_ptr<Shader>> m_Shaders{};

};