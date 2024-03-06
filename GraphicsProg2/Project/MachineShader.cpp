#include "MachineShader.h"

MachineShader::MachineShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
	: m_VertexShaderFile{vertexShaderFile}
	, m_FragmentShaderFile{fragmentShaderFile}
{
}

void MachineShader::Initialize(const VkDevice& m_Device)
{
	m_ShaderStages.clear();
	m_ShaderStages.emplace_back(CreateVertexShaderInfo(m_Device));
	m_ShaderStages.emplace_back(CreateFragmentShaderInfo(m_Device));
}

void MachineShader::DestroyShaderStages(const VkDevice& m_Device)
{
	for (auto& shaderStage : m_ShaderStages)
	{
		vkDestroyShaderModule(m_Device, shaderStage.module, nullptr);
	}
}

VkPipelineShaderStageCreateInfo MachineShader::CreateFragmentShaderInfo(const VkDevice& m_Device)
{
	std::vector<char> fragShaderCode = ReadFile("shaders/shader.frag.spv");
	VkShaderModule fragShaderModule = CreateShaderModule(m_Device, fragShaderCode);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = m_FSEntryPoint.c_str();

	return fragShaderStageInfo;
}

VkPipelineShaderStageCreateInfo MachineShader::CreateVertexShaderInfo(const VkDevice& m_Device)
{
	std::vector<char> vertShaderCode = ReadFile("shaders/shader.vert.spv");
	VkShaderModule vertShaderModule = CreateShaderModule(m_Device, vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = m_VSEntryPoint.c_str();
	return vertShaderStageInfo;
}

VkPipelineVertexInputStateCreateInfo MachineShader::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo MachineShader::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

VkShaderModule MachineShader::CreateShaderModule(const VkDevice& m_Device, const std::vector<char>& code)
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

