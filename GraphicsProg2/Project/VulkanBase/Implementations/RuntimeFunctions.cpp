#include "VulkanBase/VulkanBase.h"

void VulkanBase::DrawFrame()
{
	vkWaitForFences(m_Device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(m_Device, 1, &m_InFlightFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);


	vkResetCommandBuffer(m_CommandBuffer, 0);
	RecordCommandBuffer(m_CommandBuffer, imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffer;

	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(m_PresentQueue, &presentInfo);
}

void VulkanBase::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	RecordRenderPass(imageIndex);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}

}
void VulkanBase::RecordRenderPass(uint32_t imageIndex)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_RenderPass;
	renderPassInfo.framebuffer = m_SwapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_SwapChainExtent;

	VkClearValue clearColor = { {{0.01f, 0.01f, 0.01f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	for (Mesh* mesh : m_Scene->GetMeshes())
	{
		UpdateUniformBuffer(imageIndex, mesh->GetModelMatrix());

		BindPipelineInfo(mesh->GetShaderIndex());
		BindVertexIndexBuffers(mesh->GetVertexIndexBufferIndex());
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);

		mesh->Draw(m_CommandBuffer);
	}
	
	vkCmdEndRenderPass(m_CommandBuffer);
}

void VulkanBase::BindPipelineInfo(uint32_t pipelineIndex)
{
	vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipelines[pipelineIndex]);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_SwapChainExtent.width;
	viewport.height = (float)m_SwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_SwapChainExtent;
	vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);

}
void VulkanBase::BindVertexIndexBuffers(uint32_t buffersIndex)
{
	VkBuffer vertexBuffers[] = { m_VertexBuffers[buffersIndex]};
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(m_CommandBuffer, m_IndexBuffers[buffersIndex], 0, VK_INDEX_TYPE_UINT32);
}

void VulkanBase::UpdateUniformBuffer(uint32_t currentImage, glm::mat4 meshModelMatrix)
{
	UniformBufferObject ubo{};
	ubo.model = meshModelMatrix;
	ubo.view = m_Camera.viewMatrix;
	ubo.proj = m_Camera.projectionMatrix;

	memcpy(m_UniformBufferMapped, &ubo, sizeof(ubo));
}

