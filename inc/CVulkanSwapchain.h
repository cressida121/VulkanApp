#ifndef C_VULKAN_SWAPCHAIN_H_
#define C_VULKAN_SWAPCHAIN_H_

#include <vulkan/vulkan_core.h>
#include <vector>

namespace VulkanApp {

	class CVulkanCore;
	class CVulkanSwapchain {
	public:
		CVulkanSwapchain(CVulkanCore* const pParent, const uint32_t width, const uint32_t height, VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkRenderPass renderPass);
		~CVulkanSwapchain();
		const VkSwapchainKHR GetSwapchain() const { return m_vkSwapchain; }
		const CVulkanCore* GetParent() const { return m_pParent; }
		const VkFramebuffer GetFramebuffer(const uint32_t index);

	private:
		CVulkanCore* const m_pParent; // Guaranteed to be non-null
		uint32_t m_width = 0u;
		uint32_t m_height = 0u;
		VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		VkSurfaceFormatKHR m_vkSurfaceFormat = {};
		VkSwapchainCreateInfoKHR m_swapchainCI = {};
		VkSwapchainKHR m_vkSwapchain = VK_NULL_HANDLE;
		std::vector<VkImageView> m_swapchainImageViews;
		std::vector<VkFramebuffer> m_framebuffers;
		void InitializeFramebuffer();
		bool PresentModeAvailable(VkPresentModeKHR mode) const;
		bool SurfaceFormatAvailable(VkSurfaceFormatKHR surfaceFormat) const;
	};


}



#endif // !C_VULKAN_SWAPCHAIN_H_