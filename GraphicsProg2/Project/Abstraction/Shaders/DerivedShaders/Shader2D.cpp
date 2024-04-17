#include "Shader2D.h"
#include "vulkanbase/VulkanBase.h"

void Shader2D::Initialize(const VkDevice& m_Device)
{
	m_ShaderStages.clear();
	m_ShaderStages.emplace_back(CreateVertexShaderInfo(m_Device));
	m_ShaderStages.emplace_back(CreateFragmentShaderInfo(m_Device));
}

VkPipelineShaderStageCreateInfo Shader2D::CreateFragmentShaderInfo(const VkDevice& m_Device)
{
	std::vector<char> fragShaderCode = ReadFile(m_FragmentShaderFile);
	VkShaderModule fragShaderModule = CreateShaderModule(m_Device, fragShaderCode);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = m_FSEntryPoint.c_str();

	return fragShaderStageInfo;
}

VkPipelineShaderStageCreateInfo Shader2D::CreateVertexShaderInfo(const VkDevice& m_Device)
{
	std::vector<char> vertShaderCode = ReadFile(m_VertexShaderFile);
	VkShaderModule vertShaderModule = CreateShaderModule(m_Device, vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = m_VSEntryPoint.c_str();
	return vertShaderStageInfo;
}

VkPipelineVertexInputStateCreateInfo Shader2D::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo Shader2D::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

VkShaderModule Shader2D::CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

std::vector<VkDescriptorSetLayoutBinding> Shader2D::CreateDescriptorSetLayoutBindings()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	return { uboLayoutBinding };
}

void Shader2D::SetupDescriptorSet(VulkanBase* vulkanBase, Mesh3D* mesh)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = vulkanBase->GetUniformBuffers()[mesh->GetMeshIndex()];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);


	std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = vulkanBase->GetMeshDescriptorSets()[mesh->GetMeshIndex()];
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(vulkanBase->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

