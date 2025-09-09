#include <CVulkanPass.h>
#include <CVulkanCore.h>
#include <Utilities.h>
#include <fstream>

VulkanApp::CVulkanPass::CVulkanPass(const CVulkanCore *const pCore, const VkFormat surfaceFormat)
	: m_pCore(pCore)
{
	m_attachmentDesc.format = surfaceFormat;
	m_attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	m_attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	m_attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	m_attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	m_attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	m_attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	m_attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	m_colorAttachmentRef.attachment = 0;
	m_colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	m_subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	m_subpassDesc.colorAttachmentCount = 1;
	m_subpassDesc.pColorAttachments = &m_colorAttachmentRef;

	m_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	m_dependency.dstSubpass = 0;
	m_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	m_dependency.srcAccessMask = 0;
	m_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	m_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	m_renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	m_renderPassCI.attachmentCount = 1;
	m_renderPassCI.pAttachments = &m_attachmentDesc;
	m_renderPassCI.subpassCount = 1;
	m_renderPassCI.pSubpasses = &m_subpassDesc;
	m_renderPassCI.dependencyCount = 1;
	m_renderPassCI.pDependencies = &m_dependency;

	std::optional<uint32_t> qfIndex = VulkanApp::CapsInfo::GetQueueFamilies(m_pCore->GetVkPhysicalDevice(), VK_QUEUE_GRAPHICS_BIT);
	m_vkCommandPoolCI.queueFamilyIndex = qfIndex.value();
	m_vkCommandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	m_vkCommandPoolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	m_vkCommandBufferCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	m_vkCommandBufferCI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	m_vkCommandBufferCI.commandBufferCount = 1;
	m_vkCommandBufferCI.commandPool = VK_NULL_HANDLE;

	Initialize();
}

VulkanApp::CVulkanPass::~CVulkanPass() {
	Release();
}

void VulkanApp::CVulkanPass::Initialize() {
	Release();
	VkResult result = vkCreateRenderPass(m_pCore->GetVkLogicalDevice(), &m_renderPassCI, nullptr, &m_vkRenderPass);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot create render pass", result));
	}

	result = vkCreateCommandPool(m_pCore->GetVkLogicalDevice(), &m_vkCommandPoolCI, nullptr, &m_vkCommandBufferCI.commandPool);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot create a command pool", result));
	}

	result = vkAllocateCommandBuffers(m_pCore->GetVkLogicalDevice(), &m_vkCommandBufferCI, &m_vkCommandBuffer);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Cannot allocate a command buffer", result));
	}
}

void VulkanApp::CVulkanPass::Release() {
	vkDeviceWaitIdle(m_pCore->GetVkLogicalDevice());

	if (m_vkCommandBufferCI.commandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(m_pCore->GetVkLogicalDevice(), m_vkCommandBufferCI.commandPool, nullptr);
		m_vkCommandBufferCI.commandPool = VK_NULL_HANDLE;
		m_vkCommandBuffer = VK_NULL_HANDLE;
	}

	if (m_vkRenderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(m_pCore->GetVkLogicalDevice(), m_vkRenderPass, nullptr);
		m_vkRenderPass = VK_NULL_HANDLE;
	}
}

void VulkanApp::CVulkanPass::SubmitWorkload(
	VkQueue queue,
	VkBuffer vertexBuffer,
	VkPipeline pipeline,
	VkSemaphore waitSemaphore,
	VkSemaphore signalSemaphore,
	VkFence raiseFence,
	VkFramebuffer renderTarget,
	VkRect2D renderArea) {

	vkResetCommandBuffer(m_vkCommandBuffer, 0);

	VkCommandBufferBeginInfo beginInfoCI = {};
	beginInfoCI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfoCI.flags = 0;
	beginInfoCI.pInheritanceInfo = nullptr;

	VkResult result = vkBeginCommandBuffer(m_vkCommandBuffer, &beginInfoCI);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to begin a command buffer", result));
	}

	VkRenderPassBeginInfo renderPassCI = {};
	renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassCI.renderPass = m_vkRenderPass;
	renderPassCI.framebuffer = renderTarget;
	renderPassCI.renderArea = renderArea;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassCI.clearValueCount = 1;
	renderPassCI.pClearValues = &clearColor;

	vkCmdBeginRenderPass(m_vkCommandBuffer, &renderPassCI, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(m_vkCommandBuffer, 0, 1, &vertexBuffer, offsets);
	vkCmdDraw(m_vkCommandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(m_vkCommandBuffer);

	result = vkEndCommandBuffer(m_vkCommandBuffer);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Failed to end a command buffer", result));
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitSemaphore; // Semaphore will be signaled by vkAcquireNextImage 
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_vkCommandBuffer;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signalSemaphore;

	result = vkQueueSubmit(queue, 1, &submitInfo, raiseFence);
	if (result != VK_SUCCESS) {
		throw std::runtime_error(UTIL_EXC_MSG_EX("Command buffers submission failed", result));
	}

}


