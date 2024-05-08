#include "ShaderRT.h"
#include "vulkanbase/VulkanBase.h"

void ShaderRT::Initialize(const VkDevice& m_Device)
{
	m_SupportsImages = true;

	m_ShaderStages.clear();
	m_ShaderStages.emplace_back(CreateRayGenShaderInfo(m_Device));
	m_ShaderStages.emplace_back(CreateRayGenShaderInfo(m_Device));
	m_ShaderStages.emplace_back(CreateRayGenShaderInfo(m_Device));
}

VkPipelineShaderStageCreateInfo ShaderRT::CreateRayGenShaderInfo(const VkDevice& m_Device)
{
	std::vector<char> shaderCode = ReadFile(m_RayGenFile);
	VkShaderModule shaderModule = CreateShaderModule(m_Device, shaderCode);

	VkPipelineShaderStageCreateInfo shaderInfo{};
	shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderInfo.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	shaderInfo.module = shaderModule;
	shaderInfo.pName = m_EntryPoint.c_str();

	return shaderInfo;
}
VkPipelineShaderStageCreateInfo ShaderRT::CreateRayMissShaderInfo(const VkDevice& m_Device)
{
	std::vector<char> shaderCode = ReadFile(m_RayMissFile);
	VkShaderModule shaderModule = CreateShaderModule(m_Device, shaderCode);

	VkPipelineShaderStageCreateInfo shaderInfo{};
	shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderInfo.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
	shaderInfo.module = shaderModule;
	shaderInfo.pName = m_EntryPoint.c_str();
	return shaderInfo;
}
VkPipelineShaderStageCreateInfo ShaderRT::CreateRayHitShaderInfo(const VkDevice& m_Device)
{
	std::vector<char> shaderCode = ReadFile(m_RayHitFile);
	VkShaderModule shaderModule = CreateShaderModule(m_Device, shaderCode);

	VkPipelineShaderStageCreateInfo shaderInfo{};
	shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderInfo.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	shaderInfo.module = shaderModule;
	shaderInfo.pName = m_EntryPoint.c_str();
	return shaderInfo;
}

VkPipelineVertexInputStateCreateInfo ShaderRT::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo ShaderRT::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

VkShaderModule ShaderRT::CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code)
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

std::vector<VkDescriptorSetLayoutBinding> ShaderRT::CreateDescriptorSetLayoutBindings()
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

void ShaderRT::SetupDescriptorSet(VulkanBase* vulkanBase, Mesh* mesh)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = vulkanBase->GetUniformBuffers()[mesh->GetMeshIndex()];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = vulkanBase->GetTextureImageViews()[mesh->GetMeshIndex()];
	imageInfo.sampler = vulkanBase->GetTextureSampler();

	VkAccelerationStructureKHR tlas = vulkanBase->GetRTBuilder().getAccelerationStructure();
	VkWriteDescriptorSetAccelerationStructureKHR descASInfo{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };
	descASInfo.accelerationStructureCount = 1;
	descASInfo.pAccelerationStructures = &tlas;


	std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = vulkanBase->GetMeshDescriptorSets()[mesh->GetMeshIndex()];
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = vulkanBase->GetMeshDescriptorSets()[mesh->GetMeshIndex()];
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = vulkanBase->GetMeshDescriptorSets()[mesh->GetMeshIndex()];
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pNext = &descASInfo;


	vkUpdateDescriptorSets(vulkanBase->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

