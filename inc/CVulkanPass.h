#ifndef  C_VULKAN_PASS_H_
#define C_VULKAN_PASS_H_

#ifndef VULKAN_H_
#include <vulkan/vulkan.h>
#endif

#include <string>
#include <vector>

namespace VulkanApp {
	class CVulkanCore;
	class CVulkanPass {
	public:
		CVulkanPass(const CVulkanCore *const pCore, const VkFormat surfaceFormat);
		~CVulkanPass();
		void Initialize();
		void Release();
		const VkRenderPass GetHandle() const { return m_vkRenderPass; };
		void SubmitWorkload(VkQueue queue,
			VkBuffer vertexBuffer,
			VkPipeline pipeline,
			VkSemaphore waitSemaphore,
			VkSemaphore signalSemaphore,
			VkFence raiseFence,
			VkFramebuffer renderTarget,
			VkRect2D renderArea);

		VkAttachmentDescription m_attachmentDesc = {};
		VkAttachmentReference m_colorAttachmentRef = {};
		VkSubpassDescription m_subpassDesc = {};
		VkSubpassDependency m_dependency{};
		VkRenderPassCreateInfo m_renderPassCI = {};
		VkCommandPoolCreateInfo m_vkCommandPoolCI = {};
		VkCommandBufferAllocateInfo m_vkCommandBufferCI = {};

	private:
		VkCommandBuffer m_vkCommandBuffer = VK_NULL_HANDLE;

		const CVulkanCore *const m_pCore = nullptr;
		VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
	};
}


#endif