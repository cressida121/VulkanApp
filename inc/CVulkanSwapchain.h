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
		void Update();
		bool SetPresentMode(const VkPresentModeKHR mode);
		bool SetImageFormat(const VkSurfaceFormatKHR surfaceFormat);
		bool SetImageSize(const uint32_t width, const uint32_t height);
		uint32_t GetNextImageIndex(VkSemaphore signalImgReady) const;
		void PresentFrame(uint32_t index, VkSemaphore waitFor) const;

	private:
		const CVulkanCore* const m_pCore; // Guaranteed to be non-null
		VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
		VkSwapchainCreateInfoKHR m_swapchainCI = {};
		VkSwapchainKHR m_vkSwapchain = VK_NULL_HANDLE;
		std::vector<VkImageView> m_swapchainImageViews;
		std::vector<VkFramebuffer> m_framebuffers;
		void InitializeFramebuffer();
		void ReleaseFramebuffer();
	};
}



#endif // !C_VULKAN_SWAPCHAIN_H_