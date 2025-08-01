#ifndef C_VULKAN_SWAPCHAIN_H_
#define C_VULKAN_SWAPCHAIN_H_

#include <vulkan/vulkan_core.h>
#include <vector>

namespace VulkanApp {

	class CVulkanCore;
	class CVulkanSwapchain {
	public:
		CVulkanSwapchain(CVulkanCore* const pParent, const uint32_t width, const uint32_t height, VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat);
		~CVulkanSwapchain();
		const std::vector<VkImage> &GetImages() const { return m_swapchainImages; }
		const std::vector<VkImageView> &GetImageViews() const { return m_swapchainImageViews; }
		const VkSwapchainKHR GetSwapchain() const { return m_vkSwapchain; }
		const CVulkanCore* GetParent() const { return m_pParent; }

	private:
		CVulkanCore *const m_pParent; // Guaranteed to be non-null
		VkSwapchainKHR m_vkSwapchain = VK_NULL_HANDLE;
		std::vector<VkImage> m_swapchainImages;
		std::vector<VkImageView> m_swapchainImageViews;
		// Move framebuffer here
	};


}



#endif // !C_VULKAN_SWAPCHAIN_H_