#pragma once

#include <vector>
#include <string>

#include "vulkan/vulkan_core.h"

class VulkanBase;
class Mesh;

class Shader 
{
public:

	explicit Shader(const std::string& rayGen, const std::string& rayMiss, const std::string& rayHit)
		: m_RayGenFile{ rayGen }
		, m_RayMissFile{ rayMiss }
		, m_RayHitFile{ rayHit }
	{
	}

	virtual ~Shader() = default;

	virtual void Initialize(const VkDevice& m_Device) = 0;


	auto& GetShaderStages() { return m_ShaderStages; }
	void DestroyShaderStages(const VkDevice& m_Device)
	{
		for (auto& shaderStage : m_ShaderStages)
		{
			vkDestroyShaderModule(m_Device, shaderStage.module, nullptr);
		}
	}

	bool SupportsImages() const { return m_SupportsImages; }

	virtual VkPipelineShaderStageCreateInfo CreateRayGenShaderInfo(const VkDevice& m_Device) = 0;
	virtual VkPipelineShaderStageCreateInfo CreateRayMissShaderInfo(const VkDevice& m_Device) = 0;
	virtual VkPipelineShaderStageCreateInfo CreateRayHitShaderInfo(const VkDevice& m_Device) = 0;

	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo() = 0;
	virtual VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo() = 0;

	virtual VkShaderModule CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code) = 0;
	virtual std::vector<VkDescriptorSetLayoutBinding> CreateDescriptorSetLayoutBindings() = 0;

	virtual void SetupDescriptorSet(VulkanBase* vulkanBase, Mesh* mesh) = 0;


protected:

	std::vector<uint32_t> m_ReferencedMeshIndices{};

	std::string m_RayGenFile;
	std::string m_RayMissFile;
	std::string m_RayHitFile;

	std::string m_EntryPoint{ "main" };

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

	bool m_SupportsImages{ false };
};