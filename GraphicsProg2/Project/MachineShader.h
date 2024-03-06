#ifndef MACHINESHADER_H
#define MACHINESHADER_H

#include "vulkan/vulkan_core.h"
#include <string>
#include <vector>
#include <array>
#include "vulkanbase/VulkanUtil.h"
#include "glm/glm.hpp"
#include "labwork/Mesh.h"


const std::vector<Vertex> VERTICES = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};


class MachineShader
{
public:

	explicit MachineShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
	~MachineShader() = default;


	void Initialize(const VkDevice& m_Device);
	
	auto& GetShaderStages() { return m_ShaderStages; }
	void DestroyShaderStages(const VkDevice& m_Device);

	VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo(const VkDevice& m_Device);
	VkPipelineShaderStageCreateInfo CreateVertexShaderInfo(const VkDevice& m_Device);

	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
	
	VkShaderModule CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code);
	
private:

	std::string m_FragmentShaderFile;
	std::string m_FSEntryPoint{ "main" };
	
	std::string m_VertexShaderFile;
	std::string m_VSEntryPoint{ "main" };

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
};

#endif // !MACHINESHADER_H