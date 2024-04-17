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

class Shader2D : public Shader
{
public:

	explicit Shader2D(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		: Shader(vertexShaderFile, fragmentShaderFile) {}

	~Shader2D() = default;

	virtual void Initialize(const VkDevice& m_Device) override;

	virtual VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo(const VkDevice& m_Device) override;
	virtual VkPipelineShaderStageCreateInfo CreateVertexShaderInfo(const VkDevice& m_Device) override;

	virtual VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo() override;
	virtual VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo() override;

	virtual VkShaderModule CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code) override;
	virtual std::vector<VkDescriptorSetLayoutBinding> CreateDescriptorSetLayoutBindings() override;

	virtual void SetupDescriptorSet(VulkanBase* vulkanBase, Mesh3D* mesh) override;


};
