#include "Shader3D.h"
#include "vulkanbase/VulkanBase.h"

void Shader3D::Initialize(const VkDevice& m_Device)
{
	m_SupportsImages = true;

	m_ShaderStages.clear();
	m_ShaderStages.emplace_back(CreateVertexShaderInfo(m_Device));
	m_ShaderStages.emplace_back(CreateFragmentShaderInfo(m_Device));
}

VkPipelineShaderStageCreateInfo Shader3D::CreateFragmentShaderInfo(const VkDevice& m_Device)
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

VkPipelineShaderStageCreateInfo Shader3D::CreateVertexShaderInfo(const VkDevice& m_Device)
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

VkPipelineVertexInputStateCreateInfo Shader3D::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo Shader3D::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

VkShaderModule Shader3D::CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code)
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

std::vector<VkDescriptorSetLayoutBinding> Shader3D::CreateDescriptorSetLayoutBindings()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	return { uboLayoutBinding, samplerLayoutBinding };
}

void Shader3D::SetupDescriptorSet(VulkanBase* vulkanBase, IRenderable* renderable, uint32_t instanceID)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = vulkanBase->GetUniformBuffers()[renderable->GetRenderID()][instanceID];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = vulkanBase->GetTextureImageViews()[renderable->GetRenderID()];
	imageInfo.sampler = vulkanBase->GetTextureSampler();


	std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = vulkanBase->GetMeshDescriptorSets()[renderable->GetRenderID()][instanceID];
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = vulkanBase->GetMeshDescriptorSets()[renderable->GetRenderID()][instanceID];
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(vulkanBase->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

