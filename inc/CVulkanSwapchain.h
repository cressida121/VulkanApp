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

	//private:
		CVulkanCore *const m_pParent;
		VkSwapchainKHR m_vkSwapchain = VK_NULL_HANDLE;
		std::vector<VkImage> m_swapchainImages;
		std::vector<VkImageView> m_scImageViews;

	};


}



#endif // !C_VULKAN_SWAPCHAIN_H_