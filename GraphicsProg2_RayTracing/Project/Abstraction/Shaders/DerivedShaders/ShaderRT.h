#pragma once

#include "vulkan/vulkan_core.h"
#include <string>
#include <vector>
#include <array>
#include "VulkanUtil/VulkanUtil.h"
#include "glm/glm.hpp"
#include "Abstraction/Mesh.h"

#include "../Shader.h"
class VulkanBase;

class ShaderRT : public Shader
{
public:

	explicit ShaderRT(const std::string& rayGen, const std::string& rayMiss, const std::string& rayHit)
		: Shader(rayGen, rayMiss, rayHit) {}

	~ShaderRT() = default;

	virtual void Initialize(const VkDevice& m_Device) override;
	
	virtual VkPipelineShaderStageCreateInfo CreateRayGenShaderInfo(const VkDevice& m_Device) override;
	virtual VkPipelineShaderStageCreateInfo CreateRayMissShaderInfo(const VkDevice& m_Device) override;
	virtual VkPipelineShaderStageCreateInfo CreateRayHitShaderInfo(const VkDevice& m_Device) override;

	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo() override;
	virtual VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo() override;
	
	virtual VkShaderModule CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code) override;
	virtual std::vector<VkDescriptorSetLayoutBinding> CreateDescriptorSetLayoutBindings() override;

	virtual void SetupDescriptorSet(VulkanBase* vulkanBase, Mesh* mesh) override;

private:


};
