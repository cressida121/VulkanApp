#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>


namespace VulkanApp {
	class Renderer;
	class Application {
		friend class Renderer;
	public:
		Application(const uint32_t windowWidth, const uint32_t windowHeight);
		~Application();
		void Run();
		void RenderFrame();

	private:
		static VkInstance CreateVkInstance(
			const std::string& appName,
			const std::vector<std::string>& extensions,
			const std::vector<std::string>& layers);
		static VkDevice CreateVkLogicalDevice(VkPhysicalDevice physicalDevice, VkQueue &outGraphicsQueue, VkQueueFlags queueFlags);
		static VkSwapchainKHR CreateVkSwapchain(
			VkPhysicalDevice physicalDevice,
			VkDevice logicalDevice,
			VkSurfaceKHR surface, 
			VkSurfaceCapabilitiesKHR surfaceCapabilities,
			VkSurfaceFormatKHR requiredFormat);
		static std::vector<VkImageView> CreateImageViews(VkDevice logicalDevice, const std::vector<VkImage>& images, VkFormat format);
		bool RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t rendererIndex = 0u);

		void* m_mainWindowHandle = nullptr;
		VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
		VkInstance m_vkInstance = VK_NULL_HANDLE;
		VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_vkLogicalDevice = VK_NULL_HANDLE;
		VkQueue m_vkGraphicsQueue = VK_NULL_HANDLE;
		VkSwapchainKHR m_vkSwapchain = VK_NULL_HANDLE;
		std::vector<VkImage> m_swapchainImages;
		std::vector<VkImageView> m_scImageViews;
		VkSurfaceFormatKHR m_vkSurfaceFormat;
		VkExtent2D m_swapchainExtent;
		VkCommandPool m_gfxCommandPool;
		VkCommandBuffer m_gfxCommandBuffer; // Destroyed along with VkCommandPool
		VkSemaphore m_imageAvailableSem = VK_NULL_HANDLE;
		VkSemaphore m_renderFinishedSem = VK_NULL_HANDLE;
		VkFence m_inFlightFence = VK_NULL_HANDLE;

		std::vector<Renderer*> m_renderers;
	};
}