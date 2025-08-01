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
		CVulkanPass(CVulkanCore *pParent, std::vector<VkImageView> renderTargets, const uint32_t fbWidth, const uint32_t fbHeight);
		~CVulkanPass();
		void Initialize();
		void Release();
		void SetupFramebuffers(std::vector<VkImageView> renderTargets);
		const VkRenderPass GetRenderPass() const { return m_vkRenderPass; };

		VkAttachmentDescription m_attachmentDesc = {};
		VkAttachmentReference m_colorAttachmentRef = {};
		VkSubpassDescription m_subpassDesc = {};
		VkSubpassDependency m_dependency{};
		VkRenderPassCreateInfo m_renderPassCI = {};

	private:
		CVulkanCore* m_pCore = nullptr;
		VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> m_vkFramebuffers;
		uint32_t m_fbWidth = 0;
		uint32_t m_fbHeight = 0;

		void ReleaseFramebuffers();
	};
}


#endif