#ifndef C_VULKAN_SWAPCHAIN_H_
#define C_VULKAN_SWAPCHAIN_H_

#include <vulkan/vulkan_core.h>
#include <vector>

namespace VulkanApp {

	class CVulkanCore;
	class CVulkanSwapchain {
	public:
		CVulkanSwapchain(const CVulkanCore* const pCore, const uint32_t width, const uint32_t height, const VkSurfaceKHR surface, const VkSurfaceFormatKHR surfaceFormat, const VkRenderPass renderPass);
		~CVulkanSwapchain();
		const VkSwapchainKHR GetHandle() const { return m_vkSwapchain; }
		const CVulkanCore* GetCore() const { return m_pCore; }
		const VkFramebuffer GetFramebuffer(const uint32_t index);
		bool PresentModeAvailable(const VkPresentModeKHR mode) const;
		bool SurfaceFormatAvailable(const VkSurfaceFormatKHR surfaceFormat) const;

	private:
		const CVulkanCore* const m_pCore; // Guaranteed to be non-null
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
	};


}



#endif // !C_VULKAN_SWAPCHAIN_H_